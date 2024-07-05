## github - replit

mkdir 72-multiple_inclusion
git init
git remote add origin https://github.com/leafyoung/FN6805.git
git sparse-checkout init --cone
git sparse-checkout set 72-multiple_inclusion
git fetch
git pull origin main

git add 72-multiple_inclusion
git commit -m "72-multiple_inclusion"

git rebase origin/main
git push -u origin main