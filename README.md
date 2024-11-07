# UDP_Socket_Programming
Përshkrimi i projektit të dytë – Rrjetat Kompjuterike

Serveri
1. Të vendosen variabla te cilat përmbajnë numrin e portit (numri i portit të jetë i
çfarëdoshëm) dhe IP adresën;
2. Të jetë në gjendje të dëgjojë (listën) të paktën të gjithë anëtaret e grupit. Nëse numri i
lidhjeve kalon një prag të caktuar, serveri duhet të refuzojë lidhjet e reja ose t'i vë në pritje;
3. Të menaxhojë kërkesat e pajisjeve që dërgojnë request (ku secili anëtar i grupit duhet
ta ekzekutojë të paktën një kërkesë në server) dhe t’i logojë të gjitha për auditim të
mëvonshëm, duke përfshirë timestamp dhe IP-në e dërguesit;
4. Të jetë në gjendje të lexoje mesazhet që dërgohen nga klientët dhe t’i ruajë për monitorim;
5. Nëse një klient nuk dërgon mesazhe brenda një periudhe të caktuar kohe, serveri duhet ta
mbyllë lidhjen dhe të jetë në gjendje ta rikuperojë atë automatikisht nëse klienti rifutet;
6. Të jetë në gjendje të jap qasje të plotë të paktën njërit klient për qasje në folderat/
përmbajtjen në file-t në server.

Klienti
1. Të krijohet socket lidhja me server;
2. Njëri nga pajisjet (klientët) të ketë privilegjet write(), read(), execute() (qasje të plotë;
execute() përfshin ekzekutimin e komandave të ndryshme në server);
3. Klientët tjerë të kenë vetëm read() permission;
4. Të behet lidhja me serverin duke përcaktuar saktë portin dhe IP Adresën e serverit;
5. Të definohen saktë socket-at e serverit dhe lidhja të mos dështojë;
6. Të jetë në gjendje të lexojë përgjigjet që i kthehen nga serveri;
7. Të dërgojë mesazh serverit në formë të tekstit;
8. Të ketë qasje të plotë në folderat/përmbajtjen në server;
9. Klientët me privilegje të plota të kenë kohë përgjigjeje më të shpejtë se klientët e tjerë që
kanë vetëm read permission.

