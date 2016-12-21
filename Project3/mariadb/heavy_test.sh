run/bin/mysqlslap --create-schema=test --query="INSERT INTO food VALUES (0, 'Fried Chicken', 6)" --concurrency=20 --number-of-queries=10000 --iterations=5

run/bin/mysql -e "DELETE FROM food;" test

