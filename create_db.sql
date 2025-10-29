CREATE DATABASE IF NOT EXISTS stormbets;

CREATE TABLE IF NOT EXISTS `stormbets`.`players` (
  `id` INT NOT NULL,
  `coins` INT NOT NULL,
  `first_name` VARCHAR(45) NOT NULL,
  `last_name` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE);
    
CREATE TABLE IF NOT EXISTS `stormbets`.`match_results` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(20) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE,
  UNIQUE INDEX `name_UNIQUE` (`name` ASC) VISIBLE);
  
insert into stormbets.match_results(name) values("W1");
insert into stormbets.match_results(name) values("W2");
insert into stormbets.match_results(name) values("X");

CREATE TABLE IF NOT EXISTS `stormbets`.`matches` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `team1` VARCHAR(45) NOT NULL,
  `team2` VARCHAR(45) NOT NULL,
  `koef_w1` DOUBLE NOT NULL,
  `koef_x` DOUBLE NOT NULL,
  `koef_w2` DOUBLE NOT NULL,
  `api_id` INT NOT NULL,
  `time` VARCHAR(45) NOT NULL,
  `match_result_id` INT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE,
  UNIQUE INDEX `api_id_UNIQUE` (`api_id` ASC) VISIBLE,
  INDEX `fk_matches_1_idx` (`match_result_id` ASC) VISIBLE,
  CONSTRAINT `fk_matches_1`
    FOREIGN KEY (`match_result_id`)
    REFERENCES `stormbets`.`match_results` (`id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE);
    
CREATE TABLE IF NOT EXISTS `stormbets`.`bets` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `amount` INT NOT NULL,
  `koef` DOUBLE NOT NULL,
  `match_result_id` INT NOT NULL,
  `match_id` INT NOT NULL,
  `player_id` INT NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE,
  INDEX `fk_bets_1_idx` (`match_result_id` ASC) VISIBLE,
  INDEX `fk_bets_2_idx` (`match_id` ASC) VISIBLE,
  INDEX `fk_bets_3_idx` (`player_id` ASC) VISIBLE,
  CONSTRAINT `fk_match_results`
    FOREIGN KEY (`match_result_id`)
    REFERENCES `stormbets`.`match_results` (`id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_matches`
    FOREIGN KEY (`match_id`)
    REFERENCES `stormbets`.`matches` (`id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_players`
    FOREIGN KEY (`player_id`)
    REFERENCES `stormbets`.`players` (`id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE);

select * from matches;
select * from bets;
