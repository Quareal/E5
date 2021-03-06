# E5 - Framework &amp; CMS

Простая система для разработки сложных сайтов

##Для разработчика

###Разработка без рутины

Создание модуля выглядит как конструктор, в котором разработчик соединяет и настраивает заготовки.

Для работы с моделью данных присутствуют шаблоны моделей (нужно просто подобрать шаблон таблицы, а если в ней чего-то не хватает - дополнить её с помощью шаблонов полей).

Для работы с логикой используются компоненты. Если какой-то компонент пришлось создавать, его всегда можно использовать в других проектах.

Отображение регулируется шаблонами, в создании которых также участвуют компоненты. Универсальные шаблоны (навигации, сетки, пагинаторы) в два клика помогут отобразить любые данные из вашего модуля.

Однажды созданные, эти элементы можно использовать в дальнейшем. Забудьте о CopyPaste.

Надоело писать миграции? Встроенный менеджер зависимостей автоматически подтянет нужное при обновлении или экспорте ваших решений в другие копии системы.

Надоело учить скучный API? Предлагаем встроенный редактор шаблонов, тесно связанный с данными и логикой именно вашего разрабатываемого модуля.

Замучил роутинг? Система из коробки предоставляет инструменты для генерации и разбора URL (даже очень-очень сложных, многоуровневых URL).

Надоело организовывать backend? В системе он организовывается в момент создания модели. Вам остаётся только разделить права пользователей и настроить формы редактирования.

###Низкий порог вхождения

Концепция системы заключается в следующей идее: если вы способны изложить свои требования на бумаге, вы можете с такой же лёгкостью (и за такое же время) осуществить их в системе.

Если логика вашего модуля когда-нибудь выйдет за рамки имеющихся компонентов, то вам пригодится понимание циклов и условий из программирования.

Вам не понадобятся никакие дополнительные инструменты, кроме браузера. Весь цикл разработки протекает в веб-интерфейсе административного кабинета. Все экраны системы подробно описаны в [справочнике](http://rucms.org/helpdesk).

Дополнительные видеоматериалы демонстрируют процесс разработки типовых решений. После просмотра любого из этих материалов (~30мин) вы станете разработчиком.

Благодаря встроенному [интерактивному редактору шаблонов](http://rucms.org/helpdesk/admin_panel/work_with_module/tpl_editor) отпадает необходимость изучения API и запоминания созданных структур данных.

###Быстрое развёртывание

Система устанавливается из одного файла (php-gzip архив весом от 2 до 12мб, в зависимости от комплектации), после чего её можно обновлять с сервера обновлений, а также загружать новые модули и компоненты.

Для работы рекомендуется поднять основную копию системы на своём сервере и настроить на ней свой сервер обновлений (достаточно просто активировать уже имеющийся).

После этого, для установки системы новому клиенту, просто клонируете имеющуюся систему из настроек. В этом случае вы получаете установочный файл, содержащий ваш набор модулей и компонентов. Но что важнее - система, которая из него установится, будет обновляться не с центрального сервера, а с вашего.

Таким образом вы сможете одномоментно публиковать свои правки или нововведения для всех клиентов.

Для совсем ленивых есть режим, позволяющий произвести установку системы на другой сервер из административного кабинета. Для этого нужно ввести реквизиты FTP и MySQL, после чего произойдёт переброска и установка ядра системы. Вам останется только перейти по ссылке в административный кабинет реципиента.

###Безболезненная поддержка

Ковыряться в давно забытом коде или переписать всё с чистого листа? Кто имел дело с длительной поддержкой сложных проектов тяжело вздохнут на этом месте.

Концепция системы предполагает безболезненное расширение модели и логики проекта на любом этапе его существования. Прозрачность и концепция “конечного автомата” в большинстве случаев убирает необходимость тестирования и позволяет осуществлять модернизацию на живом проекте, без его остановки.


##Для пользователя

###Быстрая работа с контентом

Система предоставляет набор инструментов, позволяющих ускорить и упростить работу с контентом сайта.

Навигация позволяет в несколько кликов получить доступ к необходимому материалу (даже если работа проводится с множеством сайтов).

Данные можно редактировать напрямую из списка материалов (массовое редактирование).

Меню групповых операций позволяет совершить нужные действия с множеством материалов.

Фильтр позволяет искать материалы по множеству критериев (в настройках каждого модуля можно редактировать критерии поиска).

Буфер обмена позволяет переносить материалы из одного места в другое (в т.ч. с сайта на сайт).

Гибкая настройка прав доступа позволяет видеть пользователю только то, что действительно нужно.


##Для владельца сайта

###Надёжность

**Архивация данных**

Система позволяет настроить архивацию базы данных и файлов пользователя (1 раз в сутки). Архивацию можно запускать вручную, а также создать установочную копию системы со всеми данными для быстрого переноса на другой сервер.

**Контроль нагрузки**

Из панели администратора можно увидеть нагрузку сервера и количество свободного ОЗУ (в некоторых конфигурациях возможно также выводить общее кол-во ОЗУ).

Система оповестит вас в том случае, если нагрузка превысит максимально допустимую. Кроме этого, в случаях перегрузки можно отправлять пользователям 503 ошибку.

Для защиты от DDoS атак в систему встроена возможность просмотра активности IP-адресов и их блокировки.

###Скорость

В систему встроено "жёсткое" кеширование, позволяющее отдавать контент пользователю со скоростью статики.

В таком режиме обычный хостинг может выдержать до миллиона посетителей каждый день (в обычном режиме без кеша - до ста тысяч).

###Универсальность

**Мультисайтовость**

Система позволяет работать с неограниченным числом сайтов и доменов одновременно.

Контент можно переносить между сайтами, или же содержать один и тот же контент на нескольких сайтах одновременно.
