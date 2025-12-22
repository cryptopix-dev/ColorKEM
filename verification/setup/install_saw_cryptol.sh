#!/bin/bash

# Script to install SAW and Cryptol on Linux (Ubuntu/Debian)
# This script installs the necessary dependencies and tools for formal verification

set -e

echo "Installing SAW and Cryptol for formal verification..."

# Update package list
sudo apt update

# Install Haskell toolchain
echo "Installing Haskell toolchain..."
sudo apt install -y ghc cabal-install

# Update cabal
cabal update

# Install SMT solvers required by SAW
echo "Installing SMT solvers..."
sudo apt install -y z3 yices2 cvc4

# Install Cryptol
echo "Installing Cryptol..."
cabal install cryptol

# Install SAW
echo "Installing SAW..."
cabal install saw

# Add cabal bin to PATH
echo 'export PATH=$HOME/.cabal/bin:$PATH' >> ~/.bashrc
source ~/.bashrc

echo "Installation complete!"
echo "You may need to restart your shell or run 'source ~/.bashrc' to use the tools."
echo "Verify installation with: cryptol --version && saw --version"