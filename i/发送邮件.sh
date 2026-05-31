#!/bin/bash
# Send build notification email via SMTP
# Usage: bash /workspace/i/发送邮件.sh

SUBJECT="[Saba] Android APK Build Success - Build #9"
TO="xueyixuan2026@mail.xueyixuan2026.us.ci"
FROM="trae-cn.ai@xueyixuan2026.us.ci"

python3 << PYEOF
import smtplib, socket
from email.mime.text import MIMEText
from email.header import Header

socket.setdefaulttimeout(30)

msg = MIMEText("""Saba MMD Viewer - Android APK Build Notification

Status: SUCCESS

Build #9 (commit: ba7eed5) completed successfully.
APK: 14.2 MB (app-debug.apk from GitHub Actions)
All CI steps passed after 9 attempts.
Artifact ID: 7295455517

Output files in environment branch:
- Android source: /workspace/app/
- GoBot framework: /workspace/gobot/
- Viewer features: /workspace/viewer/
- Unity source: /workspace/unity-saba-viewer/

Repository: https://github.com/XUE2026/saba-android
""")

msg['Subject'] = Header('$SUBJECT', 'utf-8')
msg['From'] = '$FROM'
msg['To'] = '$TO'

try:
    server = smtplib.SMTP('smtp.qiye.aliyun.com', 25, timeout=30)
    server.starttls()
    server.login('trae-cn.ai@xueyixuan2026.us.ci', 'traecn-ai@mail!%-123-123')
    server.send_message(msg)
    server.quit()
    print('Email sent successfully!')
except Exception as e:
    print(f'Send failed: {e}')
    print('Please manually forward the notification from i/构建通知邮件.md')
PYEOF