import React from "react";

export default function App() {
  const handleClick = () => {
    alert("Button clicked");
  };

  return (
    <div>
      <input type="text" placeholder="text" />

      <input type="password" placeholder="secret password" />

      <input type="range" min={0} max={100} value={50} />

      <input type="checkbox" />

      <select>
        <option value="option1">Option 1</option>
        <option value="option2">Option 2</option>
        <option value="option3">Option 3</option>
      </select>

      <button onClick={handleClick}>Submit</button>
    </div>
  );
}
