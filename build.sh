#!/bin/bash

# Создаем необходимую структуру директорий
mkdir -p program.app/Contents/MacOS
mkdir -p program.app/Contents/Frameworks
mkdir -p program.app/Contents/Resources

# Создаем Info.plist
cat > program.app/Contents/Info.plist << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>program</string>
    <key>CFBundleIdentifier</key>
    <string>com.example.program</string>
    <key>CFBundleName</key>
    <string>program</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
</dict>
</plist>
EOF

# Компилируем программу
g++ main.cpp -o program.app/Contents/MacOS/program -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system

# Копируем фреймворки
cp -R /opt/homebrew/lib/libsfml-* program.app/Contents/Frameworks/

# Запускаем программу
./program.app/Contents/MacOS/program 