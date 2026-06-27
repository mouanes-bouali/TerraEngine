import os
import sys
import argparse
from pathlib import Path

CODE_EXTENSIONS = {
    '.py', '.cs', '.js', '.ts', '.jsx', '.tsx', '.html', '.css', 
    '.json', '.xml', '.yml', '.yaml', '.sql', '.sh', '.bat', '.ps1',
    '.cpp', '.c', '.h', '.hpp', '.java', '.go', '.rs', '.swift',
    '.kt', '.rb', '.php', '.md', '.txt', '.cfg', '.ini', '.env',
    '.gitignore', '.dockerignore', '.puml'
}

EXCLUDE_DIRS = {
    '__pycache__', '.git', '.venv', 'venv', 'env', 'node_modules',
    '.idea', '.vscode', 'dist', 'build', 'target', '__MACOSX',
    'lib', 'include', 'bin', 'obj', 'Library', 'Temp', 'Logs'
}

def should_include_file(file_path: Path) -> bool:
    if file_path.stat().st_size > 1_000_000:
        print(f"Skipping large file: {file_path} (>1 MB)")
        return False
    return file_path.suffix in CODE_EXTENSIONS

def gather_files(root_dir: Path) -> list:
    files = []
    for root, dirs, filenames in os.walk(root_dir):
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        for filename in filenames:
            file_path = Path(root) / filename
            if should_include_file(file_path):
                files.append(file_path)
    return files

def read_file_content(file_path: Path) -> str:
    encodings = ['utf-8', 'latin-1', 'cp1252']
    for enc in encodings:
        try:
            with open(file_path, 'r', encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    return f"[Binary or unreadable file: {file_path}]"

def output_to_file(files: list, output_file: str) -> None:
    with open(output_file, 'w', encoding='utf-8') as out:
        for file_path in sorted(files):
            rel_path = file_path.resolve()
            out.write(f"\n\n{'='*80}\nFILE: {rel_path}\n{'='*80}\n")
            try:
                content = read_file_content(file_path)
                out.write(content)
            except Exception as e:
                out.write(f"[ERROR reading file: {e}]")
    print(f"Output written to {output_file}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("directory", nargs="?", default=".", help="Root directory to scan")
    parser.add_argument("-o", "--output", default="code_dump.txt", help="Output file name")
    args = parser.parse_args()

    root = Path(args.directory).resolve()
    if not root.is_dir():
        print(f"Error: {root} is not a directory.")
        sys.exit(1)

    print(f"Scanning {root} ...")
    files = gather_files(root)
    print(f"Found {len(files)} files to include.")
    if not files:
        print("No matching files found.")
        return

    output_to_file(files, args.output)

if __name__ == "__main__":
    main()