#!/bin/bash
set -e

# Clean old dist directory and root binaries
rm -rf dist
rm -f "EaglePatchAC1.asi" "EaglePatchAC2.asi"

echo "Creating release directory structure..."
mkdir -p "dist/EaglePatch+AC1/scripts"
mkdir -p "dist/EaglePatch+AC2/scripts"

# Generate compile_commands.json for clangd / IDE integration
python3 -c '
import json, os
repo = os.path.abspath(".")
base_flags = [
    "clang++", "-target", "i686-pc-windows-msvc", "-m32", "-nostdlib",
    "-fms-compatibility", "-fmsc-version=1900", "-fms-extensions", "-fasm-blocks",
    "-isystem", "/usr/include/wine/msvcrt", "-isystem", "/usr/include/wine/windows",
    "-I" + os.path.join(repo, "patcher"), "-I" + os.path.join(repo, "EaglePatch/shared"),
    "-DWIN32_LEAN_AND_MEAN", "-D_CRT_SECURE_NO_WARNINGS", "-D_CRT_NONSTDC_NO_DEPRECATE",
    "-D_USE_32BIT_TIME_T", "-DNOMINMAX", "-D_MBCS", "-D_NO_CRT_STDIO_INLINE"
]
entries = [
    ("EaglePatch/src/ac1.cpp", "EaglePatchAC1"),
    ("EaglePatch/src/ac2.cpp", "EaglePatchAC2"),
    ("patcher/patcher.cpp", "EaglePatch"),
    ("patcher/crt_sections.cpp", "EaglePatch"),
    ("EaglePatch/shared/console.cpp", "EaglePatch"),
    ("EaglePatch/shared/ini_reader.cpp", "EaglePatch"),
    ("EaglePatch/shared/utils.cpp", "EaglePatch"),
    ("EaglePatch/shared/logger.cpp", "EaglePatch"),
    ("tests/test_ini_reader.cpp", "EaglePatchTests"),
]
cc = [{"directory": repo, "file": os.path.join(repo, p), "arguments": list(base_flags) + [f"-DDLL_NAME=\"{d}\"", "-c", os.path.join(repo, p)]} for p, d in entries]
with open("compile_commands.json", "w") as f:
    json.dump(cc, f, indent=2)
' 2>/dev/null || true

# Determine target toolchain parameters
if [ -d "/usr/i686-w64-mingw32/include" ]; then
    TARGET_FLAGS="--target=i686-w64-mingw32"
    SYS_INCLUDES=""
    EXTRA_LIBS=""
    TEST_FLAGS="--target=i686-w64-mingw32"
    TEST_INCLUDES=""
    TEST_EXTRA_LIBS=""
else
    TARGET_FLAGS="-target i686-pc-windows-msvc -nostdlib -Wl,-entry:DllMainCRTStartup@12 -Wl,-safeseh:no -fms-compatibility -fmsc-version=1900"
    SYS_INCLUDES="-isystem /usr/include/wine/msvcrt -isystem /usr/include/wine/windows"
    EXTRA_LIBS="/usr/lib/wine/i386-windows/libwinecrt0.a /usr/lib/wine/i386-windows/libmsvcrt.a /usr/lib/wine/i386-windows/libkernel32.a"
    TEST_FLAGS="-target i686-pc-windows-msvc -nostdlib -Wl,-entry:mainCRTStartup -Wl,-safeseh:no -fms-compatibility -fmsc-version=1900"
    TEST_INCLUDES="-isystem /usr/include/wine/msvcrt -isystem /usr/include/wine/windows"
    TEST_EXTRA_LIBS="patcher/crt_sections.cpp /usr/lib/wine/i386-windows/libwinecrt0.a /usr/lib/wine/i386-windows/libmsvcrt.a /usr/lib/wine/i386-windows/libkernel32.a"
fi

echo "Building EaglePatchAC1.asi..."
clang++ -shared -O2 $TARGET_FLAGS -fuse-ld=lld \
    -fms-extensions -fasm-blocks $SYS_INCLUDES \
    -Ipatcher \
    -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_USE_32BIT_TIME_T -DNOMINMAX -D_MBCS -D_NO_CRT_STDIO_INLINE \
    -DDLL_NAME=\"EaglePatchAC1\" \
    -o "dist/EaglePatch+AC1/scripts/EaglePatchAC1.asi" \
    patcher/crt_sections.cpp \
    patcher/patcher.cpp \
    EaglePatch/shared/console.cpp \
    EaglePatch/shared/ini_reader.cpp \
    EaglePatch/shared/utils.cpp \
    EaglePatch/shared/logger.cpp \
    EaglePatch/src/ac1.cpp $EXTRA_LIBS

echo "Packaging EaglePatch+AC1..."
cp "EaglePatch/EaglePatchAC1.ini" "dist/EaglePatch+AC1/scripts/"
cp "EaglePatch/Readme - EaglePatch+AC1.txt" "dist/EaglePatch+AC1/"

echo "Building EaglePatchAC2.asi..."
clang++ -shared -O2 $TARGET_FLAGS -fuse-ld=lld \
    -fms-extensions -fasm-blocks $SYS_INCLUDES \
    -Ipatcher \
    -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_USE_32BIT_TIME_T -DNOMINMAX -D_MBCS -D_NO_CRT_STDIO_INLINE \
    -DDLL_NAME=\"EaglePatchAC2\" \
    -o "dist/EaglePatch+AC2/scripts/EaglePatchAC2.asi" \
    patcher/crt_sections.cpp \
    patcher/patcher.cpp \
    EaglePatch/shared/console.cpp \
    EaglePatch/shared/ini_reader.cpp \
    EaglePatch/shared/utils.cpp \
    EaglePatch/shared/logger.cpp \
    EaglePatch/src/ac2.cpp $EXTRA_LIBS

echo "Packaging EaglePatch+AC2..."
cp "EaglePatch/EaglePatchAC2.ini" "dist/EaglePatch+AC2/scripts/"
cp "EaglePatch/Readme - EaglePatch+AC2.txt" "dist/EaglePatch+AC2/"

echo "Building Unit Tests..."
clang++ -O2 $TEST_FLAGS -fuse-ld=lld \
    -fms-extensions $TEST_INCLUDES \
    -IEaglePatch/shared -Ipatcher \
    -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_NO_CRT_STDIO_INLINE \
    -o "dist/EaglePatchTests.exe" \
    tests/test_ini_reader.cpp \
    EaglePatch/shared/ini_reader.cpp \
    EaglePatch/shared/utils.cpp \
    EaglePatch/shared/logger.cpp \
    $TEST_EXTRA_LIBS

if command -v wine >/dev/null 2>&1 && [ -f "dist/EaglePatchTests.exe" ]; then
    echo "Running Unit Tests with Wine..."
    WINEDEBUG=-all wine dist/EaglePatchTests.exe
fi

# Clean up temporary linker export files (.lib, .exp) to leave exactly 2 release files
rm -f dist/EaglePatch+AC1/scripts/*.lib dist/EaglePatch+AC1/scripts/*.exp
rm -f dist/EaglePatch+AC2/scripts/*.lib dist/EaglePatch+AC2/scripts/*.exp
rm -f test.lib

echo "Creating release zip archives..."
python3 -c "import shutil; shutil.make_archive('dist/EaglePatch+AC1', 'zip', 'dist/EaglePatch+AC1'); shutil.make_archive('dist/EaglePatch+AC2', 'zip', 'dist/EaglePatch+AC2')"

echo "Build successful! Release archives packaged in dist/EaglePatch+AC1.zip and dist/EaglePatch+AC2.zip"
