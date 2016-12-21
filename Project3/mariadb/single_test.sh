run/bin/mysqlslap --create-schema=test --query="INSERT INTO food VALUES (0, 'Fried Chicken', 6)" --concurrency=1 --number-of-queries=1 --iterations=1

run/bin/mysql -e "DELETE FROM food;" test

