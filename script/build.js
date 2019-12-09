const cp = require("child_process");
const path = require("path");

function build() {
  if (process.platform === "win32") {
    console.error("Can't build for windows yet");
  } else {
    cp.execSync("autoreconf -vfi", {cwd: path.resolve(__dirname, "..", "third_party", "oniguruma"), stdio: "inherit"});
    cp.execSync("./configure", {cwd: path.resolve(__dirname, "..", "third_party", "oniguruma"), stdio: "inherit"});
  }
}

build();
