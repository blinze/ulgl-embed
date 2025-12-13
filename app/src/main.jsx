import React from "react";
import { createRoot } from "react-dom/client";

function App() {
  return <h1 className="text-4xl font-bold text-red-500">Hello from bun + ultralight</h1>;
}

createRoot(document.getElementById("root")).render(<App />);
