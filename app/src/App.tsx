import React, { useState } from "react";
import NativeComponent from "./NativeComponent";

export default function App() {
  const [showTriangle, setShowTriangle] = useState(true);
  const [showComponent2, setShowComponent2] = useState(false);
  const [showComponent3, setShowComponent3] = useState(false);
  const [sliderValue, setSliderValue] = useState(50);

  return (
    <div className="p-4">
      <div className="mb-4">
        <button 
          onClick={() => setShowTriangle(!showTriangle)}
          className="px-4 py-2 mr-2 bg-blue-500 text-white rounded"
        >
          {showTriangle ? "Hide" : "Show"} Triangle
        </button>
        <button 
          onClick={() => setShowComponent2(!showComponent2)}
          className="px-4 py-2 mr-2 bg-green-500 text-white rounded"
        >
          {showComponent2 ? "Hide" : "Show"} Component 2
        </button>
        <button 
          onClick={() => setShowComponent3(!showComponent3)}
          className="px-4 py-2 bg-red-500 text-white rounded"
        >
          {showComponent3 ? "Hide" : "Show"} Component 3
        </button>
      </div>
      {showTriangle && (
        <NativeComponent 
          name="triangle" 
          style={{ width: '400px', height: '300px' }}
        />
      )}
      {showComponent2 && (
        <div className="mb-4">
          <input
            type="range"
            min="0"
            max="100"
            value={sliderValue}
            onChange={(e) => setSliderValue(Number(e.target.value))}
            className="w-64"
          />
          <span className="ml-2">{sliderValue}</span>
        </div>
      )}
      {showComponent3 && (
        <p className="mb-4">
          lmao some text cause i cant be bothered to open lorem ipsum
        </p>
      )}
    </div>
  );
}
