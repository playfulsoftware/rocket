#ifndef ROCKET_CRTP_UTILS_H
#define ROCKET_CRTP_UTILS_H

#define IMPLEMENTS(impl, iface) friend class iface<impl>

#endif // ROCKET_CRTP_UTILS_H
