/*
 * This file is part of the telepathy-nonsense connection manager.
 * Copyright (C) 2015 Niels Ole Salscheider <niels_ole@salscheider-online.de>
 * Copyright (C) 2014 Alexandr Akulich <akulichalexander@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */

#include "protocol.hh"
#include "connection.hh"
#include "common.hh"

#include <TelepathyQt/BaseConnection>
#include <TelepathyQt/Constants>
#include <TelepathyQt/RequestableChannelClassSpec>
#include <TelepathyQt/RequestableChannelClassSpecList>
#include <TelepathyQt/Types>
#include <TelepathyQt/ProtocolParameterList>
#include <TelepathyQt/Utils>

#include <QXmppUtils.h>


Protocol::Protocol(const QDBusConnection &dbusConnection, const QString &name)
    : Tp::BaseProtocol(dbusConnection, name)
{
    setRequestableChannelClasses(Tp::RequestableChannelClassSpecList() << Tp::RequestableChannelClassSpec::textChat());
    setEnglishName(QLatin1String("XMPP"));
    setIconName(QLatin1String("im-jabber"));
    setVCardField(QLatin1String("impp"));

    setCreateConnectionCallback(Tp::memFun(this, &Protocol::createConnection));
    setIdentifyAccountCallback(Tp::memFun(this, &Protocol::identifyAccount));
    setNormalizeContactCallback(Tp::memFun(this, &Protocol::normalizeContact));

    setParameters(Tp::ProtocolParameterList() << Tp::ProtocolParameter(QLatin1String("account"), QDBusSignature(QLatin1String("s")), 0)//Tp::ConnMgrParamFlagRequired | Tp::ConnMgrParamFlagRegister)
                                              //<< Tp::ProtocolParameter(QLatin1String("password"), QDBusSignature(QLatin1String("s")), Tp::ConnMgrParamFlagSecret) // | Tp::ConnMgrParamFlagRequired | Tp::ConnMgrParamFlagRegister)
                                              //<< Tp::ProtocolParameter(QLatin1String("server"), QDBusSignature(QLatin1String("s")), 0) //Tp::ConnMgrParamFlagRequired | Tp::ConnMgrParamFlagRegister)
                                              << Tp::ProtocolParameter(QLatin1String("resource"), QDBusSignature(QLatin1String("s")), 0) //Tp::ConnMgrParamFlagRequired)
                                              << Tp::ProtocolParameter(QLatin1String("priority"), QDBusSignature(QLatin1String("u")), Tp::ConnMgrParamFlagHasDefault, 0)); //Tp::ConnMgrParamFlagRequired

    m_addrIface = Tp::BaseProtocolAddressingInterface::create();
    m_addrIface->setAddressableVCardFields(QStringList() << QLatin1String("impp"));
    m_addrIface->setAddressableUriSchemes(QStringList() << QLatin1String("xmpp"));
    m_addrIface->setNormalizeVCardAddressCallback(Tp::memFun(this, &Protocol::normalizeVCardAddress));
    m_addrIface->setNormalizeContactUriCallback(Tp::memFun(this, &Protocol::normalizeContactUri));
    plugInterface(Tp::AbstractProtocolInterfacePtr::dynamicCast(m_addrIface));

    m_avatarsIface = Tp::BaseProtocolAvatarsInterface::create();
    m_avatarsIface->setAvatarDetails(Common::getAvatarSpec());
    plugInterface(Tp::AbstractProtocolInterfacePtr::dynamicCast(m_avatarsIface));

    m_presenceIface = Tp::BaseProtocolPresenceInterface::create();
    m_presenceIface->setStatuses(Tp::PresenceSpecList(Common::getSimpleStatusSpecMap()));
    plugInterface(Tp::AbstractProtocolInterfacePtr::dynamicCast(m_presenceIface));
}

Tp::BaseConnectionPtr Protocol::createConnection(const QVariantMap &parameters, Tp::DBusError *error)
{
    Q_UNUSED(error)

    Tp::BaseConnectionPtr newConnection = Tp::BaseConnection::create<Connection>(QLatin1String("nonsense"), name(), parameters);

    return newConnection;
}

QString Protocol::identifyAccount(const QVariantMap &parameters, Tp::DBusError *error)
{
    Q_UNUSED(error)

    return Tp::escapeAsIdentifier(parameters[QLatin1String("account")].toString());
}

QString Protocol::normalizeContact(const QString &contactId, Tp::DBusError *error)
{
    Q_UNUSED(error)

    return QXmppUtils::jidToBareJid(contactId);
}

QString Protocol::normalizeVCardAddress(const QString &vcardField, const QString vcardAddress,
        Tp::DBusError *error)
{
    DBG;
    error->set(QLatin1String("NormalizeVCardAddress.Error.Test"), QLatin1String(""));
    return QString();
}

QString Protocol::normalizeContactUri(const QString &uri, Tp::DBusError *error)
{
    DBG;
    error->set(QLatin1String("NormalizeContactUri.Error.Test"), QLatin1String(""));
    return QString();
}
