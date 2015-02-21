# battery-notify
Notifies the user when battery hits critical levels with notify-send

battery-notify uses uevent in the sys/class/power_supply and notify-send to alert the user when their battery levels have falled bellow 20% and 10%. Useful for when there is no desktop environment to take care of it for you. Battery-notify uses a patched version of notify-osd which can be installed on ubuntu systems easily by adding ppa:leolik/leolik to your list of ppas and upgrading notify-osd.
