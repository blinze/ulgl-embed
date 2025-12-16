export type PrimitiveType = "cube" | "pyramid" | "cylinder" | "cone";

interface NativeBridge {
  setComponentSlot(name: string, x: number, y: number, width: number, height: number, visible: boolean): void;
  setRotation(x: number, y: number, z: number): void;
  setPrimitive(type: PrimitiveType): void;

  [key: string]: (...args: any[]) => any;
}

declare global {
  interface Window {
    native: NativeBridge;
  }
}

export {};
