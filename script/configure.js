const cp = require("child_process");
const path = require("path");

function configure() {
  if (process.platform === "win32") {
    cp.execSync('"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" && make_win.bat', {cwd: path.resolve(__dirname, "..", "third_party", "oniguruma"), stdio: "inherit"});
  } else {
    cp.execSync("autoreconf -vfi && ./configure", {cwd: path.resolve(__dirname, "..", "third_party", "oniguruma"), stdio: "inherit"});
  }
}

configure();
