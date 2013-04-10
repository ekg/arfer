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

    variantFile.addHeaderLine("##INFO=<ID=AFmle,Number=A,Type=Integer,Description=\"Allele frequency estimated from GLs.\">");
    variantFile.addHeaderLine("##INFO=<ID=AFmle_ref,Number=1,Type=Integer,Description=\"Reference allele frequency estimated from GLs.\">");

    cout << variantFile.header << endl;

    double eps = 1e-20;

    Variant var(variantFile);
    while (variantFile.getNextVariant(var)) {
        vector<vector<double> > GLs;
        vector<double> mleHWEAlleleFreq;
        uint32_t N;  // should be == # of samples with data
        extractProbsFromGLs(var, GLs);
        if (estimateHWEAlleleFrequencies(GLs, eps, mleHWEAlleleFreq, N, var.alleles.size())) {
            vector<string>::iterator a = var.alleles.begin();
            vector<double>::iterator f = mleHWEAlleleFreq.begin();
            var.info["AFmle_ref"].push_back(convert(*f));
            ++a; ++f;
            vector<string>& afmles = var.info["AFmle"];
            for ( ; f != mleHWEAlleleFreq.end(); ++f, ++a) {
                afmles.push_back(convert(*f));
            }
        }
        cout << var << endl;
    }

    return 0;

}
