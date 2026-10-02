{
  lib,
  stdenv,
  src,
  cmake,
  ninja,
  makeWrapper,
  fetchFromGitHub,
  copyDesktopItems,
  makeDesktopItem,
  qtbase,
  qtmultimedia,
  qttools,
  qtwayland,
  wrapQtAppsHook,
  ## Keep in sync with project() in src/CMakeLists.txt.
  version ? "0.2.2",
}:

let
  ## Vendored upstream as the git submodule src/codecconvert/ced. Flake sources
  ## carry no submodule contents, so fetch it separately and copy it into place.
  ## Rev must match `git submodule status src/codecconvert/ced`.
  ced = fetchFromGitHub {
    owner = "google";
    repo = "compact_enc_det";
    rev = "d127078cedef9c6642cbe592dacdd2292b50bb19";
    hash = "sha256-5P7X8aNZ+9PlL12IqcejXSSDwQlgC2dIbt1BmnWeLfw=";
  };

  ## src/CMakeLists.txt appends the lowercased CMAKE_CXX_COMPILER_ID to the
  ## executable name, normalizing AppleClang to clang.
  exeName = "qitools-${if stdenv.cc.isClang then "clang" else "gnu"}";

  ## main.cpp loads translations/ and qitoolswindow.cpp loads plugins/ relative
  ## to applicationDirPath(), so the real binary must sit next to both.
  appDir = "$out/share/qitools";
in
stdenv.mkDerivation {
  pname = "qitools";
  inherit version src;

  nativeBuildInputs = [
    cmake
    ninja
    makeWrapper
    qttools
    wrapQtAppsHook
  ]
  ++ lib.optional stdenv.hostPlatform.isLinux copyDesktopItems;

  buildInputs = [
    qtbase
    qtmultimedia
  ]
  ++ lib.optional stdenv.hostPlatform.isLinux qtwayland;

  ## The top-level CMakeLists.txt only forwards to src/.
  cmakeDir = "../src";

  cmakeFlags = [
    ## Produce a plain binary instead of a .app so installPhase is uniform.
    (lib.cmakeBool "QITOOLS_MAC_BUNDLE" false)
  ];

  postPatch = ''
    mkdir -p src/codecconvert/ced
    cp -r --no-preserve=mode,ownership ${ced}/. src/codecconvert/ced/
  '';

  ## EXECUTABLE_OUTPUT_PATH points back into the source tree at bin/, which is
  ## writable here, but nothing installs the plugins — do it by hand.
  installPhase = ''
    runHook preInstall

    install -Dm755 ../bin/${exeName} ${appDir}/qitools
    install -Dm555 -t ${appDir}/plugins ../bin/plugins/*${stdenv.hostPlatform.extensions.sharedLibrary}
    install -Dm444 -t ${appDir}/translations ../bin/translations/*.qm
    install -Dm444 -t ${appDir} ../resource/QiTools.css
    install -Dm444 ../resource/toolsimage.svg $out/share/icons/hicolor/scalable/apps/qitools.svg

    runHook postInstall
  '';

  ## wrapQtAppsHook would descend into share/qitools/plugins and try to wrap the
  ## plugin libraries, so drive the wrapper manually.
  dontWrapQtApps = true;

  postFixup = ''
    makeWrapper ${appDir}/qitools $out/bin/qitools "''${qtWrapperArgs[@]}"
  '';

  desktopItems = lib.optional stdenv.hostPlatform.isLinux (makeDesktopItem {
    name = "qitools";
    exec = "qitools";
    icon = "qitools";
    desktopName = "QiTools";
    comment = "Personal common tools collection";
    categories = [ "Utility" ];
  });

  doCheck = true;

  checkPhase = ''
    runHook preCheck
    QT_QPA_PLATFORM=offscreen ctest --output-on-failure
    runHook postCheck
  '';

  meta = {
    description = "Cross-platform Qt tool collection with a plugin architecture";
    homepage = "https://github.com/handy-sun/qitools";
    license = lib.licenses.lgpl3Plus;
    platforms = lib.platforms.unix;
    mainProgram = "qitools";
  };
}
