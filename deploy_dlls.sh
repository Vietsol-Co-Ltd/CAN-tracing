#!/bin/bash

# Đường dẫn đến file .exe
EXE="./cantracing.exe"

# Kiểm tra tồn tại
if [[ ! -f "$EXE" ]]; then
    echo "Không tìm thấy $EXE. Vui lòng chạy script từ thư mục chứa cantracing.exe"
    exit 1
fi

# Thư mục đích (mặc định là hiện tại)
DEST_DIR="."

# Lấy danh sách các .dll bằng ldd
echo "📦 Đang phân tích $EXE với ldd..."
ldd "$EXE" | grep "ucrt64" | awk '{print $3}' | while read dll; do
    if [[ -f "$dll" ]]; then
        echo "🔄 Copy: $(basename "$dll")"
        cp -u "$dll" "$DEST_DIR/"
    else
        echo "⚠️ Không tìm thấy: $dll"
    fi
done

echo "✅ Hoàn tất copy DLL vào $DEST_DIR"
