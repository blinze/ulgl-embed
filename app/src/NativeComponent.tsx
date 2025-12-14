import React, { useRef, useEffect, useCallback } from "react";

declare global {
  interface Window {
    native?: {
      setComponentSlot: (
        name: string,
        x: number,
        y: number,
        width: number,
        height: number,
        visible: boolean
      ) => void;
    };
  }
}

interface NativeComponentProps {
  name: string;
  className?: string;
  style?: React.CSSProperties;
  children?: React.ReactNode;
}

export default function NativeComponent({ name, className, style, children }: NativeComponentProps) {
  const ref = useRef<HTMLDivElement>(null);

  const updateSlot = useCallback(() => {
    if (ref.current && window.native) {
      const rect = ref.current.getBoundingClientRect();
      window.native.setComponentSlot(
        name,
        rect.left,
        rect.top,
        rect.width,
        rect.height,
        true
      );
    }
  }, [name]);

  useEffect(() => {
    updateSlot();

    window.addEventListener("resize", updateSlot);
    const interval = setInterval(updateSlot, 100);

    return () => {
      window.removeEventListener("resize", updateSlot);
      clearInterval(interval);
      
      if (window.native)
        window.native.setComponentSlot(name, 0, 0, 0, 0, false);
    };
  }, [name, updateSlot]);

  return (
    <div ref={ref} className={className} style={style}>
      {children}
      {!window.native && (
        <div className="text-sm">
          [Native: {name}]
        </div>
      )}
    </div>
  );
}
