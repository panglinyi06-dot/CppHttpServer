#!/bin/bash
echo "Content-type: text/html; charset=utf-8"
echo ""

echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "<meta charset='utf-8'>"
echo "<title>注册成功</title>"
echo "<style>"
echo "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }"
echo ".container { max-width: 600px; margin: auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }"
echo "h1 { color: #4CAF50; }"
echo "table { width: 100%; border-collapse: collapse; margin: 20px 0; }"
echo "th { background: #4CAF50; color: white; padding: 10px; text-align: left; }"
echo "td { padding: 10px; border-bottom: 1px solid #ddd; }"
echo ".back-btn { display: inline-block; padding: 10px 20px; background: #4CAF50; color: white; text-decoration: none; border-radius: 5px; }"
echo ".back-btn:hover { background: #45a049; }"
echo "</style>"
echo "</head>"
echo "<body>"
echo "<div class='container'>"
echo "<h1>✅ 注册成功！</h1>"

if [ "$REQUEST_METHOD" = "POST" ]; then

    POST_DATA=$(cat)

    echo "<table>"
    echo "<tr><th>字段</th><th>值</th></tr>"

    IFS='&' read -ra PARAMS <<< "$POST_DATA"

    for pair in "${PARAMS[@]}"; do

        key=$(echo "$pair" | cut -d'=' -f1)
        value=$(echo "$pair" | cut -d'=' -f2-)

        value=$(echo -e "$(echo "$value" | sed 's/+/ /g; s/%/\\x/g')")

        echo "<tr><td>$key</td><td>$value</td></tr>"
    done

    echo "</table>"

else
    echo "<p>没有收到POST数据</p>"
fi

echo "<hr>"
echo "<a href='post.html' class='back-btn'>← 返回重新填写</a>"
echo "</div>"
echo "</body>"
echo "</html>"
