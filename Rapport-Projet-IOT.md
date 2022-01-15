# Rapport Projet IOT

### Sommaire
* [Objectifs du Projet](#objectifs-du-projet)
* [Architecture globale du réseau de sirènes](#architecture-globale-du-réseau-de-sirènes)
* [Sécurité globale](#sécurité-globale)
* [Architecture matérielle](#architecture-matérielle)
* [Coût de la BOM (Bill Of Materials) pour 5000 unités](#coût-de-la-bom-pour-5000-unités)
* [Coût des certifications](#coût-des-certifications)
* [Implémentation logicielle](#implémentation-logicielle)
* [Format LPP des messages](#format-lpp-des-messages)
* [Logiciel embarqué de l'objet sirène](#logiciel-embarqué-de-lobjet-sirène)
* [Métriques logicielles](#métriques-logicielles)
* [Changement de comportement de l'objet en fonction des évènements](#changement-de-comportement-de-lobjet-en-fonction-des-évènements)
* [Durée de vie de la batterie](#durée-de-vie-de-la-batterie)
* [Analyse du Cycle de Vie du produit](#analyse-du-cycle-de-vie-du-produit)
* [Avantages/Inconvénients des produits concurrents](#avantagesinconvénients-des-produits-concurrents)
* [Solutions utilisables pour localiser l'objet sirène](#solutions-utilisables-pour-localiser-lobjet-sirène)
* [Intégrations effectuées](#intégrations-effectuées)

## Objectifs du Projet
Le but de ce projet était de créer un détecteur d'incendie. Pour ce faire, nous avions à notre disposition une carte LoRa, une carte ST et des capteurs nécessaires pour détecter la présence d'un incendie (capteur de température, d'humidité, ...).
De plus, ce projet avait pour but de mettre en évidence la communication LoRa entre les mesures détectées et un serveur dédié (nous avons choisi d'utiliser Cayenne).

## Architecture globale du réseau de sirènes

## Sécurité globale

## Architecture matérielle

## Coût de la BOM pour 5000 unités
Coût de la carte LoRa : 26.90€.
Coût du capteur SCD30 : 68,80€.
Total : 95,70€.
Nous n'avons pas utilisé de buzzer pour ce projet car nous avons d'abord préféré nous concentrer sur la communication LoRa entre le serveur et notre système.

## Coût des certifications
Pour la certification ETSI du produit, le coût serait de 300€/an. 
La certification LoRa Alliance coûte 10000$/an.

## Implémentation logicielle

## Format LPP des messages
Pour communiquer entre notre objet et notre serveur, nous envoyons des trames de messages. Ces messages sont découpés en 3 parties :
* La température qui va être mesurée par le capteur scd30 sur le channel 1.
* L'humidité qui va être mesurée par le capteur scd30 sur le channel 2.
* La concentration en CO2 qui va être mesurée par le capteur scd30 sur le channel 3.

## Logiciel embarqué de l'objet sirène

## Métriques logicielles

## Changement de comportement de l'objet en fonction des évènements

## Durée de vie de la batterie

## Analyse du Cycle de Vie du produit

## Avantages/Inconvénients des produits concurrents

## Solutions utilisables pour localiser l'objet sirène

## Intégrations effectuées
