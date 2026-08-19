#!/usr/bin/env bash
set -e

REPO_DIR="/home/pi/Duy/Hethonggiamsat"
cd "$REPO_DIR"

echo "=== Staging files ==="
git add .gitignore .gitattributes .editorconfig .clang-format
git add LICENSE CONTRIBUTING.md CODE_OF_CONDUCT.md SECURITY.md CHANGELOG.md HARDWARE.md README.md
git add data/.gitkeep logs/.gitkeep
git add esp32/include/secrets.h.example
git add .github/

echo "=== Git Status ==="
git status

echo "=== Committing changes ==="
git commit -m "docs & ci: establish standardized technical GitHub repository structure

- Add MIT License, CONTRIBUTING.md, CODE_OF_CONDUCT.md, SECURITY.md, CHANGELOG.md
- Add comprehensive HARDWARE.md detailing pinouts and BOM
- Add GitHub Actions CI workflows for C++ lint, Qt6 host build, and PlatformIO ESP32 build
- Add Release workflow, Dependabot config, Issue Forms and PR Template
- Add .clang-format and .editorconfig for consistent code quality
- Enhance .gitignore and .gitattributes
- Provide secrets.h.example and runtime directory placeholders (.gitkeep)
- Update README.md with status badges, system architecture diagram, and full documentation" || echo "Nothing to commit or already committed"

echo "=== Pushing to remote repository ==="
git push origin main || git push origin master || echo "Push completed or requires credentials"

echo "=== Done ==="
