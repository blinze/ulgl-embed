#!/usr/bin/env python3

import sys
import os
import shutil
import stat

def copy_file_safe(src_file, dst_file):
    try:
        os.makedirs(os.path.dirname(dst_file), exist_ok=True)

        if os.path.exists(dst_file):
            try:
                os.chmod(dst_file, stat.S_IWRITE)
                os.remove(dst_file)
            except (PermissionError, OSError):
                pass

        shutil.copy2(src_file, dst_file)
        return True
    except (PermissionError, OSError) as e:
        print(f"  Skipping locked file: {os.path.basename(src_file)}", file=sys.stderr)
        return False
    except Exception as e:
        print(f"  Warning: Could not copy {os.path.basename(src_file)}: {e}", file=sys.stderr)
        return False

def copy_directory(src, dst, exclude_dirs=None, exclude_files=None):
    if not os.path.exists(src):
        print(f"Warning: Source directory does not exist: {src}", file=sys.stderr)
        return 1

    if not os.path.isdir(src):
        print(f"Error: Source is not a directory: {src}", file=sys.stderr)
        return 1

    if exclude_dirs is None:
        exclude_dirs = ['node_modules', '.git', '.vscode', '.idea', '__pycache__', '.pytest_cache', 'src', 'public']
    if exclude_files is None:
        exclude_files = ['.gitignore', '.gitattributes', '.DS_Store', 'Thumbs.db', 'yarn.lock', 'package-lock.json', 'tsconfig.json']

    try:
        os.makedirs(dst, exist_ok=True)
    except Exception as e:
        print(f"Error: Could not create destination directory {dst}: {e}", file=sys.stderr)
        return 1

    copied_count = 0
    skipped_count = 0

    for root, dirs, files in os.walk(src):
        dirs[:] = [d for d in dirs if d not in exclude_dirs]
        
        rel_path = os.path.relpath(root, src)
        if rel_path == '.':
            dst_dir = dst
        else:
            dst_dir = os.path.join(dst, rel_path)
        try:
            os.makedirs(dst_dir, exist_ok=True)
        except Exception:
            pass

        for file in files:
            if file in exclude_files:
                skipped_count += 1
                continue
                
            src_file = os.path.join(root, file)
            dst_file = os.path.join(dst_dir, file)

            if copy_file_safe(src_file, dst_file):
                copied_count += 1
            else:
                skipped_count += 1

    if skipped_count > 0:
        print(f"Copied {copied_count} files, skipped {skipped_count} files from {src} to {dst}")
    else:
        print(f"Successfully copied {copied_count} files from {src} to {dst}")
    return 0

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python cmake_copy_dir.py <source> <destination>", file=sys.stderr)
        sys.exit(1)

    src = sys.argv[1]
    dst = sys.argv[2]

    sys.exit(copy_directory(src, dst))
