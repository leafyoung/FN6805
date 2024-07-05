## github - replit

mkdir 54-fstream_demo
git init
git remote add origin https://github.com/leafyoung/FN6805.git
git sparse-checkout init --cone
git sparse-checkout set 54-fstream_demo
git fetch
git pull origin main

git add 54-fstream_demo
git commit -m "54-fstream_demo"

git rebase origin/main
git push -u origin main