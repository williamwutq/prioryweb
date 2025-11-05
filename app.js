// Hosting setup. This will server static files only: index.html, everything in /dotcom folder
// We do not use localhost, but use 0.0.0.0

import https from "https";
import http from "http";
import fs from "fs";
import path from "path";
import url from "url";

const __dirname = path.dirname(url.fileURLToPath(import.meta.url));
const dotcomDir = path.join(__dirname, "dotcom");
const HTTPSPORT = 8888;
const HTTPPORT = 8880;

const options = {
  key: fs.readFileSync(path.join(__dirname, "cert/server.key")),
  cert: fs.readFileSync(path.join(__dirname, "cert/server.cert")),
};

// Serve static file helper
function serveFile(req, res) {
  let filePath = path.join(
    dotcomDir,
    req.url === "/" ? "index.html" : req.url
  );

  // Prevent directory traversal attacks
  if (!filePath.startsWith(dotcomDir)) {
    res.writeHead(403);
    return res.end("Forbidden");
  }

  fs.readFile(filePath, (err, data) => {
    if (err) {
      res.writeHead(404);
      return res.end("Not Found");
    }

    res.writeHead(200);
    res.end(data);
  });
}

/*** HTTPS Server ***/
https.createServer(options, serveFile).listen(HTTPSPORT, "0.0.0.0", () => {
  console.log("HTTPS server running at https://0.0.0.0:" + HTTPSPORT);
});

/*** HTTP Server ***/
http.createServer(serveFile).listen(HTTPPORT, "0.0.0.0", () => {
  console.log("HTTP server running at http://0.0.0.0:" + HTTPPORT);
});
