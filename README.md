# arfer
### annotation of variants using genotype likelihoods

#### author: Adrian Tan <atks@umich.edu>
#### author: Erik Garrison <erik.garrison@bc.edu>

#### license: MIT

---

## overview

arfer is a stream filter which consumes VCF files with per-sample genotype likelihoods encoded in the GL fields
and outputs the same records annotated with various statistical estimates that can be used for variant comparison and filtering.
These estimates are based on the likelihoods themselves, and thus are independent of the called genotypes represented
in the GT fields.  The method is capable of handling multiallelic loci.

The annotations include:

- *ACmle*, *AFmle_ref*, and *AFmle*, GL-based estimates of allele counts in the set of samples, derived via an EM procedure.
- *GLmle*, GL-based estimate of genotype frequencies, ordered according to the GL field in the [VCF specification](http://vcftools.sourceforge.net/specs.html).
- *HWEpval*, the two-tailed probability of observing a distribution of genotypes this extreme assuming
   [Hard-Weinberg Equilibrium](https://en.wikipedia.org/wiki/Hardy%E2%80%93Weinberg_principle).
- *FIC*, the [inbreeding coefficient](https://en.wikipedia.org/wiki/Coefficient_of_relationship#Inbreeding_coefficient)

The models used in arfer are described on the University of Michigan Center for Statistical Genetics wiki.
ACmle, AFmle, and GLmle are derived using [Genotype Likelihood based Allele Frequency](http://genome.sph.umich.edu/wiki/Genotype_Likelihood_based_Allele_Frequency).
HWEpval follows from [Genotype Likelihood based Hardy-Weinberg Test](http://genome.sph.umich.edu/wiki/HWEP).
And, FIC is provided via [Genotype Likelihood based Inbreeding Coefficient](http://genome.sph.umich.edu/wiki/FIC).


## installing

arfer includes submodules, so to obtain it you have to use:

    % git clone --recursive git://github.com/ekg/arfer.git

or

    % git clone --recursive https://github.com/ekg/arfer.git

To build, use Make:

    % cd arfer
    % make

The executable 'arfer' is built in the root directory of the repository.

## use

Provided a file variants.vcf which includes genotype likelihoods, simply push the file through arfer:

    cat variants.vcf | arfer >variants.annotated.vcf

The output will contain the annotations ACmle, AFmle, AFmle_ref, GLmle, HWEpval, and FIC.
