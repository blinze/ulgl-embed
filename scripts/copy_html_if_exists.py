#!/usr/bin/env python3

import sys
import os
import shutil

if len(sys.argv) != 3:
    print("Usage: python copy_html_if_exists.py <source> <destination>", file=sys.stderr)
    sys.exit(1)

src = sys.argv[1]
dst = sys.argv[2]

if os.path.exists(src):
    try:
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy2(src, dst)
        print(f"Copied {src} to {dst}")
        sys.exit(0)
    except Exception as e:
        print(f"Warning: Could not copy {src} to {dst}: {e}", file=sys.stderr)
        sys.exit(1)
else:
    sys.exit(0)

