const express = require("express");

const app = express();
const port = 3000;

// Middleware to parse JSON bodies
app.use(express.json());

// Limit the number of active connections
const MAX_CLIENTS = 5; // Set the maximum number of clients
let activeClients = 0;

// Middleware to check the number of active connections
app.use((req, res, next) => {
  if (activeClients >= MAX_CLIENTS) {
    return res.status(503).json({ message: "Server is busy. Try again later." });
  }
  activeClients++;
  console.log(activeClients)
  res.on("finish", () => {
    activeClients--;
    console.log(activeClients)
  });
  next();
});

// Key-Value store
const keyValueStore = {};

// Route to handle data from the client
app.post("/data", (req, res) => {
  const { key, value, action } = req.body;
  console.log(req.body);

  if (!key || !action) {
    return res.status(400).json({ message: "Key and action are required!" });
  }

  if (action === "set") {
    keyValueStore[key] = value;
    res.json({ message: `Key '${key}' set to '${value}'.` });
  } else if (action === "get") {
    if (key in keyValueStore) {
      res.json({ value: keyValueStore[key] });
    } else {
      res.status(404).json({ message: `Key '${key}' not found.` });
    }
  } else if (action === "delete") {
    if (key in keyValueStore) {
      delete keyValueStore[key];
      res.json({ message: `Key '${key}' deleted.` });
    } else {
      res.status(404).json({ message: `Key '${key}' not found.` });
    }
  } else {
    res.status(400).json({ message: "Invalid action." });
  }
});

// Start the server
app.listen(port, () => {
  console.log(`Server running on http://localhost:${port}`);
});
