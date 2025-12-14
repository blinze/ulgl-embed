#!/usr/bin/env python3

import sys
import os
import shutil
import stat

def get_file_hash(filepath):
    try:
        stat_info = os.stat(filepath)
        return (stat_info.st_size, stat_info.st_mtime)
    except OSError:
        return None

def should_copy_file(src, dst):
    if not os.path.exists(dst):
        return True
    
    src_hash = get_file_hash(src)
    dst_hash = get_file_hash(dst)
    
    if src_hash is None:
        return False
    
    return src_hash != dst_hash

def copy_file_incremental(src_file, dst_file):
    try:
        if not should_copy_file(src_file, dst_file):
            return False
        
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

def copy_directory_incremental(src, dst, exclude_dirs=None, exclude_files=None, use_symlinks=False):
    if not os.path.exists(src):
        print(f"Warning: Source directory does not exist: {src}", file=sys.stderr)
        return 1
    
    if not os.path.isdir(src):
        print(f"Error: Source is not a directory: {src}", file=sys.stderr)
        return 1
    
    if exclude_dirs is None:
        exclude_dirs = ['node_modules', '.git', '.vscode', '.idea', '__pycache__', '.pytest_cache']
    if exclude_files is None:
        exclude_files = ['.gitignore', '.gitattributes', '.DS_Store', 'Thumbs.db']
    
    try:
        os.makedirs(dst, exist_ok=True)
    except Exception as e:
        print(f"Error: Could not create destination directory {dst}: {e}", file=sys.stderr)
        return 1
    
    copied_count = 0
    skipped_count = 0
    unchanged_count = 0
    
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
            
            if use_symlinks:
                try:
                    if os.path.exists(dst_file) or os.path.islink(dst_file):
                        if os.path.islink(dst_file):
                            os.remove(dst_file)
                        elif os.path.isfile(dst_file):
                            os.remove(dst_file)
                    rel_src = os.path.relpath(src_file, dst_dir)
                    os.symlink(rel_src, dst_file)
                    copied_count += 1
                except (OSError, NotImplementedError):
                    if copy_file_incremental(src_file, dst_file):
                        copied_count += 1
                    else:
                        unchanged_count += 1
            else:
                if copy_file_incremental(src_file, dst_file):
                    copied_count += 1
                else:
                    unchanged_count += 1
    
    if use_symlinks:
        print(f"Created {copied_count} symlinks from {src} to {dst}")
    else:
        if unchanged_count > 0:
            print(f"Copied {copied_count} files, {unchanged_count} unchanged, skipped {skipped_count} files from {src} to {dst}")
        else:
            print(f"Copied {copied_count} files, skipped {skipped_count} files from {src} to {dst}")
    return 0

def copy_build_output_only(src_app_dir, dst_app_dir):
    src_build = os.path.join(src_app_dir, "build")
    dst_build = os.path.join(dst_app_dir, "build")
    
    if not os.path.exists(src_build):
        print(f"Warning: React build directory not found: {src_build}", file=sys.stderr)
        print(f"  Make sure to run 'npm run build' or 'yarn build' in the app directory first", file=sys.stderr)
        return 1
    
    return copy_directory_incremental(src_build, dst_build, exclude_dirs=[], exclude_files=[])

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python smart_copy.py <mode> <source> <destination> [options]", file=sys.stderr)
        print("  Modes:", file=sys.stderr)
        print("    full      - Copy entire directory (with exclusions)", file=sys.stderr)
        print("    build     - Copy only build output (for React apps)", file=sys.stderr)
        print("  Options:", file=sys.stderr)
        print("    --symlink - Use symlinks instead of copying (development only)", file=sys.stderr)
        sys.exit(1)
    
    mode = sys.argv[1]
    src = sys.argv[2]
    dst = sys.argv[3]
    use_symlinks = '--symlink' in sys.argv
    
    if mode == "build":
        sys.exit(copy_build_output_only(src, dst))
    elif mode == "full":
        sys.exit(copy_directory_incremental(src, dst, use_symlinks=use_symlinks))
    else:
        print(f"Error: Unknown mode '{mode}'", file=sys.stderr)
        sys.exit(1)

