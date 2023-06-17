set -euxo pipefail

BETTERQQNT_VERSION=${BETTERQQNT_VERSION:-0.2.0}

BETTERQQNT_VERSION_X86=${BETTERQQNT_VERSION_X86:-${BETTERQQNT_VERSION}}
BETTERQQNT_VERSION_X64=${BETTERQQNT_VERSION_X64:-${BETTERQQNT_VERSION}}

mkdir dist

mkdir build
cd build
cmake -DBETTERQQNT_ARCH=x86 -DBETTERQQNT_VERSION=${BETTERQQNT_VERSION_X86} -G 'Visual Studio 17 2022' -A Win32 -T v143 ..
cmake --build . --target bqqinj --config MinSizeRel
cd ..
cp build/MinSizeRel/bqqinj.exe dist/bqqenv-x86.exe
rm -rf build

mkdir build
cd build
cmake -DBETTERQQNT_ARCH=x64 -DBETTERQQNT_VERSION=${BETTERQQNT_VERSION_X64} -G 'Visual Studio 17 2022' -A x64 -T v143 ..
cmake --build . --target bqqinj --config MinSizeRel
cd ..
cp build/MinSizeRel/bqqinj.exe dist/bqqenv-x64.exe
rm -rf build

mkdir build
cd build
cmake -G 'Visual Studio 17 2022' -A Win32 -T v143 ..
cmake --build . --target bqqenv --config MinSizeRel
cd ..
cp build/MinSizeRel/bqqenv.exe dist/bqqenv.exe
rm -rf build
