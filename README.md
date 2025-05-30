
# gitql

gitql — лёгкий SQL-подобный язык для выборки информации из истории Git-репозитория. Позволяет быстро искать коммиты по автору, дате, файлам и сообщению.

# Примеры запросов

```sql
SELECT author_name, author_email, hash
FROM commits
WHERE 'int' IN files
  AND date IN RANGE '01.01.2021' AND '01.01.2025'
LIMIT 10
```

```sql
SELECT hash, author_name
FROM commits
WHERE author_name == 'sh4rrkyyyy'
LIMIT 1000
```

```sql
SELECT author_name, hash, date
FROM commits
WHERE year == '2010'
LIMIT 5
```

```sql
SELECT author_name, hash, date
FROM commits
WHERE month == '09.2016'
LIMIT 10
```

```sql
SELECT author_name, hash, author_email, date
FROM commits
WHERE day == '12.05.2025'
LIMIT 100
```

---
## Структура запроса

```sql
SELECT <fields>
FROM commits
WHERE <conditions>
LIMIT <quantity>
```

### Доступные поля (в любом порядке и количестве)

* `author_name` — имя автора
* `author_email` — e-mail автора
* `date` — дата коммита (формат `DD.MM.YYYY`)
* `message` — сообщение коммита
* `hash` — хеш коммита
* `*` — все поля

### WHERE: условия фильтрации

1. **Равенство**
   Любое поле можно сравнить на точное равенство:

   ```sql
   SELECT hash, author_name
   FROM commits
   WHERE author_name == 'sh4rrkyyyy'
   LIMIT 1000
   ```

   Для даты (`date`) поддерживается сравнение с точностью до:

   * **года:**

     ```sql
     SELECT author_name, hash, date
     FROM commits
     WHERE year == '2010'
     LIMIT 5
     ```
   * **месяца:**

     ```sql
     SELECT author_name, hash, date
     FROM commits
     WHERE month == '09.2016'
     LIMIT 10
     ```
   * **дня:**

     ```sql
     SELECT author_name, hash, author_email, date
     FROM commits
     WHERE day == '12.05.2025'
     LIMIT 100
     ```

2. **Поиск в диапазоне дат**

   ```sql
   SELECT author_name, author_email, hash
   FROM commits
   WHERE 'int' IN files
     AND date IN RANGE '01.01.2021' AND '01.01.2025'
   LIMIT 10
   ```

3. **Поиск по вхождению**

   ```sql
   SELECT *
   FROM commits
   WHERE 'int' IN files
   LIMIT 20
   ```

   или

   ```sql
   SELECT *
   FROM commits
   WHERE 'bugfix' IN message
   LIMIT 50
   ```

---