#!/bin/sh
#build last picotls master (for Travis)

# Build at a known-good commit
COMMIT_ID=e278d032381414641c286ae6f02c407f83c7586c

cd ..
git clone --branch master --single-branch --shallow-submodules --recurse-submodules --no-tags https://github.com/h2o/picotls
cd picotls
git checkout "$COMMIT_ID"
#git submodule init
#git submodule update
cmake $CMAKE_OPTS .
make -j$(nproc) all
cd ..
