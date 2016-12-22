run/bin/mysqlslap --create-schema=test --query="INSERT INTO food VALUES (0, 'Fried Chicken', 6)" --concurrency=30 --number-of-queries=20000 --iterations=2

run/bin/mysql -e "DELETE FROM food;" test

