REATE TABLE User (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(50) NOT NULL,
    state ENUM('online', 'offline') DEFAULT 'offline'
);

CREATE TABLE Friend (
    userid INT NOT NULL,
    friendid INT NOT NULL,
    PRIMARY KEY (userid, friendid),
    FOREIGN KEY (userid) REFERENCES User(id),
    FOREIGN KEY (friendid) REFERENCES User(id)
);

CREATE TABLE AllGroup (
    id INT PRIMARY KEY AUTO_INCREMENT,
    groupname VARCHAR(50) NOT NULL,
    groupdesc VARCHAR(200) DEFAULT ''
);

CREATE TABLE GroupUser (
    groupid INT,
    userid INT NOT NULL,
    grouprole ENUM('creator', 'normal') DEFAULT 'normal',
    PRIMARY KEY (groupid, userid),
    FOREIGN KEY (groupid) REFERENCES AllGroup(id),
    FOREIGN KEY (userid) REFERENCES User(id)
);

CREATE TABLE OfflineMessage (
    userid INT PRIMARY KEY,
    message VARCHAR(500) NOT NULL,
    FOREIGN KEY (userid) REFERENCES User(id)
);

