import React, { useState } from "react";

export default function App() {
  const [sliderValue, setSliderValue] = useState(50);

  return (
    <div>
      <input type="text" placeholder="text" />
      <input type="password" placeholder="secret password" />
      <input
        type="range"
        min={0}
        max={100}
        value={sliderValue}
        onChange={(e) => setSliderValue(Number(e.target.value))}
      />
      <span>{sliderValue}</span>
      <input type="checkbox" />

      <select>
        <option value="option1">Option 1</option>
        <option value="option2">Option 2</option>
        <option value="option3">Option 3</option>
      </select>

      <button>Submit</button>
    </div>
  );
}
