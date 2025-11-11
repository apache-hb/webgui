// Import path for resolving file paths
var path = require("path");
module.exports = {
    // Specify the entry point for our app.
    entry: [path.join(__dirname, "install/share/gui.js")],
    // Specify the output file containing our bundled code.
    output: {
        path: path.join(__dirname, "install/share/"),
        filename: '[name].js',
        chunkFilename: '[name].bundle.js',
    },
    // Enable WebPack to use the 'path' package.
    resolve: {
        fallback: {
            path: require.resolve("path-browserify"),
            crypto: false,
            fs: false
        }
    },
    optimization: {
        runtimeChunk: 'single',
    },
};