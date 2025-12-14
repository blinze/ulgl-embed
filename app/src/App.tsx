import React, { useState } from "react";
import { Button } from "components/ui/button";
import { Input } from "components/ui/input";
import { Slider } from "components/ui/slider";
import { Checkbox } from "components/ui/checkbox";
import { Label } from "components/ui/label";

export default function App() {
  const [text, setText] = useState("");
  const [password, setPassword] = useState("");
  const [sliderValue, setSliderValue] = useState(50);
  const [checked, setChecked] = useState(false);

  return (
    <div className="space-y-6 p-6 max-w-md mx-auto">
      <div className="space-y-2">
        <Label htmlFor="text">Text input</Label>
        <Input
          id="text"
          placeholder="Text"
          value={text}
          onChange={(e) => setText(e.target.value)}
        />
      </div>

      <div className="space-y-2">
        <Label htmlFor="password">Password</Label>
        <Input
          id="password"
          type="password"
          placeholder="Password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
        />
      </div>

      <div className="space-y-2">
        <Label>Value: {sliderValue}</Label>
        <Slider
          value={[sliderValue]}
          min={0}
          max={100}
          step={1}
          onValueChange={(val) => setSliderValue(val[0])}
        />
      </div>

      <div className="flex items-center space-x-2">
        <Checkbox
          id="checkbox"
          checked={checked}
          onCheckedChange={(val) => setChecked(val as boolean)}
        />
        <Label htmlFor="checkbox">Check me</Label>
      </div>

      <div className="flex space-x-4">
        <Button>Submit</Button>
        <Button
          variant="secondary"
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
    </div>
  );
}
