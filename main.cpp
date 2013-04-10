#include <iostream>
#include <algorithm>
#include "estimators.h"
#include "vcflib/Variant.h"
#include "vcflib/convert.h"

using namespace std;
using namespace vcf;

void extractProbsFromGLs(Variant& var, vector<vector<double> >& GLs) {
    for (vector<string>::iterator s = var.sampleNames.begin(); s != var.sampleNames.end(); ++s) {
        vector<double> g;
        map<string, map<string, vector<string> > >::iterator m = var.samples.find(*s);
        if (m != var.samples.end()) {
            map<string, vector<string> >& sample = m->second;
            map<string, vector<string> >::iterator l = sample.find("GL");
            if (l != sample.end()) {
                for (vector<string>::iterator i = l->second.begin(); i != l->second.end(); ++i) {
                    double x;
                    convert(*i, x);
                    x = pow(10, x);
                    g.push_back(x);
                }
            }
        }
        GLs.push_back(g);
    }
}

int main(int argc, char** argv) {

    VariantCallFile variantFile;

    if (argc > 1) {
        string filename = argv[1];
        variantFile.open(filename);
    } else {
        variantFile.open(std::cin);
    }

    if (!variantFile.is_open()) {
        return 1;
    }

    variantFile.addHeaderLine("##INFO=<ID=AFmle,Number=A,Type=Float,Description=\"Allele frequency estimated from GLs.\">");
    variantFile.addHeaderLine("##INFO=<ID=AFmle_ref,Number=1,Type=Float,Description=\"Reference allele frequency estimated from GLs.\">");
    variantFile.addHeaderLine("##INFO=<ID=GFmle,Number=G,Type=Float,Description=\"Genotype frequencies estimated from GLs.\">");
    variantFile.addHeaderLine("##INFO=<ID=HWEpval,Number=1,Type=Float,Description=\"HWE p-value estimated from GLs.\">");
    variantFile.addHeaderLine("##INFO=<ID=FIC,Number=1,Type=Float,Description=\"Inbreeding coefficient estimated from GLs.\">");

    cout << variantFile.header << endl;

    double eps = 1e-20;  // convergence limit

    Variant var(variantFile);
    while (variantFile.getNextVariant(var)) {
        vector<vector<double> > GLs;
        vector<double> mleHWEAlleleFreq;
        uint32_t N;  // should be == # of samples with data
        extractProbsFromGLs(var, GLs);
        if (estimateHWEAlleleFrequencies(GLs, eps, mleHWEAlleleFreq, N, var.alleles.size())) {
            vector<double>::iterator f = mleHWEAlleleFreq.begin();
            var.info["AFmle_ref"].clear();
            var.info["AFmle_ref"].push_back(convert(*f));
            ++f;
            vector<string>& afmles = var.info["AFmle"];
            afmles.clear();
            for ( ; f != mleHWEAlleleFreq.end(); ++f) {
                afmles.push_back(convert(*f));
            }
        } else {
            cerr << "could not estimate allele frequencies for " << var.sequenceName << ":" << var.position << endl;
            continue;
        }

        vector<double> mleGenotypeFreq;
        if (estimateGenotypeFrequencies(GLs, eps, mleGenotypeFreq, N, var.alleles.size())) {
            vector<string>& mlegfs = var.info["GFmle"];
            mlegfs.clear();
            for (vector<double>::iterator gf = mleGenotypeFreq.begin(); gf != mleGenotypeFreq.end(); ++gf) {
                mlegfs.push_back(convert(*gf));
            }
        } else {
            cerr << "could not estimate genotype frequencies for " << var.sequenceName << ":" << var.position << endl;
            continue;
        }

        double lrts; // likelihood ratio test statistic
        double pValue; // likelihood ratio p-value
        uint32_t dof;
        if (hweLRT(GLs, mleGenotypeFreq, mleHWEAlleleFreq, lrts, pValue, dof, var.alleles.size())) {
            var.info["HWEpval"].clear();
            var.info["HWEpval"].push_back(convert(pValue));
        } else {
            cerr << "could not estimate HWE likelihood ratio test for " << var.sequenceName << ":" << var.position << endl;
            continue;
        }

        double F; // inbreeding coefficient
        if (estimateFIC(GLs, mleGenotypeFreq, mleHWEAlleleFreq, F, var.alleles.size())) {
            var.info["FIC"].clear();
            var.info["FIC"].push_back(convert(F));
        } else {
            cerr << "could not estimate inbreeding coefficient for " << var.sequenceName << ":" << var.position << endl;
            continue;
        }

        cout << var << endl;
    }

    return 0;

}
