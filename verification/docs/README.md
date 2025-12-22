# Formal Verification Infrastructure for ColorKEM

This directory contains the infrastructure for formal verification of ColorKEM using SAW (Software Analysis Workbench) and Cryptol.

## Overview

Formal verification provides mathematical proofs that the implementation correctly implements the cryptographic specifications. This is crucial for high-assurance cryptography.

## Tools Used

- **Cryptol**: Domain-specific language for cryptographic algorithms
- **SAW**: Tool for verifying C/C++ code against Cryptol specifications using symbolic execution and SMT solvers

## Directory Structure

```
verification/
├── setup/              # Installation scripts
│   └── install_saw_cryptol.sh
├── cryptol/            # Cryptol specifications
│   ├── polynomial_arithmetic.cry
│   └── ntt.cry
├── saw/                # SAW verification scripts
│   └── verify_ntt.saw
├── ci/                 # CI/CD integration (placeholder)
└── docs/               # Documentation
    └── README.md
```

## Getting Started

### 1. Install Tools

Run the installation script:

```bash
./verification/setup/install_saw_cryptol.sh
```

This installs:
- Haskell toolchain (GHC, Cabal)
- Cryptol
- SAW
- SMT solvers (Z3, Yices2, CVC4)

### 2. Verify Installation

```bash
cryptol --version
saw --version
```

### 3. Run Basic Verification

```bash
# Type check Cryptol specifications
cd verification/cryptol
cryptol polynomial_arithmetic.cry
cryptol ntt.cry

# Run SAW verification (when implemented)
cd ../saw
saw verify_ntt.saw
```

## Current Status

This is the **initial infrastructure setup**. The following components are in place:

- ✅ Tool installation scripts
- ✅ Basic Cryptol specifications for polynomial arithmetic
- ✅ Framework for NTT specifications
- ✅ SAW verification script template
- ✅ CI/CD integration via GitHub Actions

### Next Steps for Full Verification

1. **Complete Cryptol Specifications**:
   - Implement full Cooley-Tukey NTT algorithm
   - Add Montgomery arithmetic specifications
   - Define proper bit reversal operations

2. **SAW Proof Development**:
   - Extract C functions from compiled LLVM bitcode
   - Prove equivalence between C implementation and Cryptol specs
   - Handle complex operations (butterfly, Montgomery reduction)

3. **Integration with Build System**:
   - Automate extraction of verification targets
   - Integrate proofs into regular testing workflow

## Workflow

1. **Specification**: Write Cryptol specs for desired properties
2. **Extraction**: Compile C code to LLVM and extract functions
3. **Verification**: Use SAW to prove C code matches specs
4. **CI/CD**: Automated verification on code changes

## Key Components to Verify

- NTT forward/inverse transforms
- Polynomial multiplication
- Montgomery modular arithmetic
- Sampling functions
- KEM key operations

## Resources

- [Cryptol Documentation](https://cryptol.net/)
- [SAW Documentation](https://saw.galois.com/)
- [Galois SAW Tutorials](https://saw.galois.com/tutorials/)

## Contributing

When adding new verification:

1. Update Cryptol specs first
2. Add corresponding SAW scripts
3. Test locally before CI/CD
4. Update this documentation
