-- taskable database setup scripts --

-- employers table
CREATE TABLE employers
(
    employer_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    is_active INTEGER NOT NULL
);

-- clients table
CREATE TABLE clients
(
    client_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    is_active INTEGER NOT NULL,

    employer_id INTEGER NOT NULL,

    FOREIGN KEY (employer_id) REFERENCES employers(employer_id)
);

-- rate_types table
CREATE TABLE rate_types
(
    rate_type_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL
);

-- create currencies table
CREATE TABLE currencies
(
    currency_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    code TEXT NOT NULL,
    symbol TEXT NOT NULL
);

-- projects table
CREATE TABLE projects
(
    project_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL UNIQUE,
    display_name TEXT NOT NULL,
    billable INTEGER NOT NULL,
    rate REAL NULL,
    hours INTEGER NULL,
    is_default INTEGER NOT NULL DEFAULT (0),
    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    is_active INTEGER NOT NULL,

    employer_id INTEGER NOT NULL,
    client_id INTEGER NULL,
    rate_type_id INTEGER NULL,
    currency_id INTEGER NULL,

    FOREIGN KEY (employer_id) REFERENCES employers(employer_id),
    FOREIGN KEY (client_id) REFERENCES clients(client_id),
    FOREIGN KEY (rate_type_id) REFERENCES rate_types(rate_type_id),
    FOREIGN KEY (currency_id) REFERENCES currencies(currency_id)
);

-- categories table
CREATE TABLE categories
(
    category_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    color INTEGER NOT NULL,
    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    is_active INTEGER NOT NULL,

    project_id INTEGER NOT NULL,

    FOREIGN KEY (project_id) REFERENCES projects(project_id)
);

-- tasks table
CREATE TABLE tasks
(
    task_id INTEGER PRIMARY KEY NOT NULL,
    task_date TEXT NOT NULL UNIQUE,
    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    is_active INTEGER NOT NULL
);

-- task item types table
CREATE TABLE task_item_types
(
    task_item_type_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL
);


-- task items table
CREATE TABLE task_items
(
    task_item_id INTEGER PRIMARY KEY NOT NULL,
    start_time TEXT NULL,
    end_time TEXT NULL,
    duration TEXT NOT NULL,
    description TEXT NOT NULL,
    billable INTEGER NOT NULL,
    calculated_rate REAL NULL,
    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),
    is_active INTEGER NOT NULL,

    task_item_type_id INTEGER NOT NULL,
    project_id INTEGER NOT NULL,
    task_id INTEGER NOT NULL,
    category_id INTEGER NOT NULL,

    FOREIGN KEY (task_item_type_id) REFERENCES task_item_types(task_item_type_id),
    FOREIGN KEY (project_id) REFERENCES projects(project_id),
    FOREIGN KEY (task_id) REFERENCES tasks(task_id),
    FOREIGN KEY (category_id) REFERENCES categories(category_id)
);
