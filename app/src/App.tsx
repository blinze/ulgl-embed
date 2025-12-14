import React, { useState } from "react";
import { Button } from "components/ui/button";
import { Input } from "components/ui/input";
import { Slider } from "components/ui/slider";
import { Checkbox } from "components/ui/checkbox";
import { Label } from "components/ui/label";
import NativeComponent from "NativeComponent";

export default function App() {
  const [text, setText] = useState("");
  const [password, setPassword] = useState("");
  const [sliderValue, setSliderValue] = useState(50);
  const [checked, setChecked] = useState(false);

  return (
    <div className="flex flex-col h-screen p-2 gap-4">
      <div className="flex items-center gap-2">
        <Input
          id="text"
          placeholder="Text"
          value={text}
          onChange={(e) => setText(e.target.value)}
        />

        <Input
          id="password"
          type="password"
          placeholder="Password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
        />

        <Label>{sliderValue}</Label>
        <Slider
          value={[sliderValue]}
          min={0}
          max={100}
          step={1}
          onValueChange={(val) => setSliderValue(val[0])}
        />

        <Checkbox
          checked={checked}
          onCheckedChange={(val) => setChecked(val as boolean)}
        />

        <Button
          variant="destructive"
          size="sm"
          onClick={() => {
            setText("");
            setPassword("");
            setSliderValue(50);
            setChecked(false);
          }}
        >
          Reset
        </Button>
      </div>
      <div className="flex-1">
        <NativeComponent name="triangle" className="h-full w-full" />
      </div>
    </div>
  );
}
