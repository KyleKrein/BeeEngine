import sys
import os
import shutil

def copy_headers(src_dir, dst_dir):
    if not os.path.isdir(src_dir):
        print(f"Error: '{src_dir}' is not a valid directory.")
        return

    for root, _, files in os.walk(src_dir):
        for file in files:
            if file.endswith(('.h', '.hpp')):
                src_file_path = os.path.join(root, file)
                relative_path = os.path.relpath(root, src_dir)
                dst_folder_path = os.path.join(dst_dir, relative_path)
                dst_file_path = os.path.join(dst_folder_path, file)

                os.makedirs(dst_folder_path, exist_ok=True)
                shutil.copy2(src_file_path, dst_file_path)
                print(f"Copied: {src_file_path} → {dst_file_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <source_directory> <destination_directory>")
        sys.exit(1)

    src = sys.argv[1]
    dst = sys.argv[2]
    copy_headers(src, dst)

