interface NativeBridge {
  setComponentSlot(name: string, x: number, y: number, width: number, height: number, visible: boolean): void;
  setCubeRotation(x: number, y: number, z: number): void;

  [key: string]: (...args: any[]) => any;
}

declare global {
  interface Window {
    native: NativeBridge;
  }
}

export {};
