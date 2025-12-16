import React, { useState } from "react";
import { Button } from "components/ui/button";
import { Input } from "components/ui/input";
import { Slider } from "components/ui/slider";
import { Checkbox } from "components/ui/checkbox";
import { Label } from "components/ui/label";
import { NativeComponent } from "./native";

export default function App() {
  const [rotationX, setRotationX] = useState(0);
  const [rotationY, setRotationY] = useState(0);
  const [rotationZ, setRotationZ] = useState(0);

  const updateRotation = (x: number, y: number, z: number) => {
    if (window.native) {
      window.native.setRotation(x, y, z);
      window.native.print(`Rotated to ${x}, ${y}, ${z}`);
    }
  };

  const handleRotationX = (val: number[]) => {
    setRotationX(val[0]);
    updateRotation(val[0], rotationY, rotationZ);
  };

  const handleRotationY = (val: number[]) => {
    setRotationY(val[0]);
    updateRotation(rotationX, val[0], rotationZ);
  };

  const handleRotationZ = (val: number[]) => {
    setRotationZ(val[0]);
    updateRotation(rotationX, rotationY, val[0]);
  };

  const resetRotation = () => {
    if (window.native) window.native.print("Reset rotation");
    setRotationX(0);
    setRotationY(0);
    setRotationZ(0);
    updateRotation(0, 0, 0);
  };

  return (
    <div className="flex flex-col h-screen p-2 gap-4">
      <div className="flex items-center gap-4">
        <div className="flex items-center gap-4">
          <Label>X:</Label>
          <Slider
            value={[rotationX]}
            min={0}
            max={360}
            step={1}
            onValueChange={handleRotationX}
            className="flex-1 w-32"
          />
          <span className="text-xs font-mono">
            {rotationX}
          </span>
        </div>

        <div className="flex items-center gap-4">
          <Label>Y:</Label>
          <Slider
            value={[rotationY]}
            min={0}
            max={360}
            step={1}
            onValueChange={handleRotationY}
            className="flex-1 w-32"
          />
          <span className="text-xs font-mono">
            {rotationY}
          </span>
        </div>

        <div className="flex items-center gap-4">
          <Label>Z:</Label>
          <Slider
            value={[rotationZ]}
            min={0}
            max={360}
            step={1}
            onValueChange={handleRotationZ}
            className="flex-1 w-32"
          />
          <span className="text-xs font-mono">
            {rotationZ}
          </span>
        </div>

        <Button variant="destructive" size="sm" onClick={resetRotation}>
          Reset
        </Button>
      </div>
      <div className="flex-1">
        <NativeComponent name="cube" className="h-full w-full" />
      </div>
    </div>
  );
}
