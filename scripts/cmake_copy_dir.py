#!/usr/bin/env python3
"""
Cross-platform directory copy script that handles locked files gracefully.
"""
import sys
import os
import shutil
import stat

def copy_file_safe(src_file, dst_file):
    """Copy a single file, handling locked files gracefully."""
    try:
        # Ensure destination directory exists
        os.makedirs(os.path.dirname(dst_file), exist_ok=True)
        
        # Try to remove destination file if it exists and is read-only
        if os.path.exists(dst_file):
            try:
                os.chmod(dst_file, stat.S_IWRITE)
                os.remove(dst_file)
            except (PermissionError, OSError):
                pass  # File might be locked, we'll try to overwrite anyway
        
        # Copy the file
        shutil.copy2(src_file, dst_file)
        return True
    except (PermissionError, OSError) as e:
        # File is locked or permission denied - this is okay, skip it
        print(f"  Skipping locked file: {os.path.basename(src_file)}", file=sys.stderr)
        return False
    except Exception as e:
        print(f"  Warning: Could not copy {os.path.basename(src_file)}: {e}", file=sys.stderr)
        return False

def copy_directory(src, dst):
    """Copy directory, handling locked files gracefully."""
    if not os.path.exists(src):
        print(f"Warning: Source directory does not exist: {src}", file=sys.stderr)
        return 1
    
    if not os.path.isdir(src):
        print(f"Error: Source is not a directory: {src}", file=sys.stderr)
        return 1
    
    # Create destination directory
    try:
        os.makedirs(dst, exist_ok=True)
    except Exception as e:
        print(f"Error: Could not create destination directory {dst}: {e}", file=sys.stderr)
        return 1
    
    # Copy files one by one
    copied_count = 0
    skipped_count = 0
    
    for root, dirs, files in os.walk(src):
        # Calculate relative path from source
        rel_path = os.path.relpath(root, src)
        if rel_path == '.':
            dst_dir = dst
        else:
            dst_dir = os.path.join(dst, rel_path)
        
        # Create subdirectories
        try:
            os.makedirs(dst_dir, exist_ok=True)
        except Exception:
            pass  # Directory might already exist
        
        # Copy files
        for file in files:
            src_file = os.path.join(root, file)
            dst_file = os.path.join(dst_dir, file)
            
            if copy_file_safe(src_file, dst_file):
                copied_count += 1
            else:
                skipped_count += 1
    
    if skipped_count > 0:
        print(f"Copied {copied_count} files, skipped {skipped_count} locked files from {src} to {dst}", file=sys.stderr)
    else:
        print(f"Successfully copied {copied_count} files from {src} to {dst}")
    
    # Always return 0 (success) even if some files were skipped
    # This allows the build to continue even when the app is running
    return 0

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python cmake_copy_dir.py <source> <destination>", file=sys.stderr)
        sys.exit(1)
    
    src = sys.argv[1]
    dst = sys.argv[2]
    
    sys.exit(copy_directory(src, dst))
