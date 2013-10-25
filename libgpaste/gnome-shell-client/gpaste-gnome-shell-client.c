/*
 *      This file is part of GPaste.
 *
 *      Copyright 2013 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
 *
 *      GPaste is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      GPaste is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with GPaste.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gpaste-gnome-shell-client-private.h"

#define __G_PASTE_NEEDS_AU__
#define __G_PASTE_NEEDS_BS__
#include "gpaste-gdbus-macros.h"

#define G_PASTE_GNOME_SHELL_BUS_NAME       "org.gnome.Shell"
#define G_PASTE_GNOME_SHELL_OBJECT_PATH    "/org/gnome/Shell"
#define G_PASTE_GNOME_SHELL_INTERFACE_NAME "org.gnome.Shell"

#define G_PASTE_GNOME_SHELL_EVAL               "Eval"
#define G_PASTE_GNOME_SHELL_FOCUS_SEARCH       "FocusSearch"
#define G_PASTE_GNOME_SHELL_SHOW_OSD           "ShowOSD"
#define G_PASTE_GNOME_SHELL_FOCUS_APP          "FocusApp"
#define G_PASTE_GNOME_SHELL_SHOW_APPLICATIONS  "ShowApplications"
#define G_PASTE_GNOME_SHELL_GRAB_ACCELERATOR   "GrabAccelerator"
#define G_PASTE_GNOME_SHELL_GRAB_ACCELERATORS  "GrabAccelerators"
#define G_PASTE_GNOME_SHELL_UNGRAB_ACCELERATOR "UngrabAccelerator"

#define G_PASTE_GNOME_SHELL_SIG_ACCELERATOR_ACTIVATED "AcceleratorActivated"

#define G_PASTE_GNOME_SHELL_PROP_MODE            "Mode"
#define G_PASTE_GNOME_SHELL_PROP_OVERVIEW_ACTIVE "OverviewActive"
#define G_PASTE_GNOME_SHELL_PROP_SHELL_VERSION   "ShellVersion"

#define G_PASTE_GNOME_SHELL_INTERFACE                                                                      \
    "<node>"                                                                                               \
        "<interface  name='" G_PASTE_GNOME_SHELL_INTERFACE_NAME "'>"                                       \
            "<method name='" G_PASTE_GNOME_SHELL_EVAL "'>"                                                 \
                "<arg type='s' direction='in'  name='script'  />"                                          \
                "<arg type='b' direction='out' name='success' />"                                          \
                "<arg type='s' direction='out' name='result'  />"                                          \
            "</method>"                                                                                    \
            "<method name='" G_PASTE_GNOME_SHELL_FOCUS_SEARCH "' />"                                       \
            "<method name='" G_PASTE_GNOME_SHELL_SHOW_OSD "'>"                                             \
                "<arg type='a{sv}' direction='in' name='params' />"                                        \
            "</method>"                                                                                    \
            "<method name='" G_PASTE_GNOME_SHELL_FOCUS_APP "'>"                                            \
                "<arg type='s' direction='in' name='id' />"                                                \
            "</method>"                                                                                    \
            "<method name='" G_PASTE_GNOME_SHELL_SHOW_APPLICATIONS "' />"                                  \
            "<method name='" G_PASTE_GNOME_SHELL_GRAB_ACCELERATOR "'>"                                     \
                "<arg type='s' direction='in'  name='accelerator' />"                                      \
                "<arg type='u' direction='in'  name='flags'       />"                                      \
                "<arg type='u' direction='out' name='action'      />"                                      \
            "</method>"                                                                                    \
            "<method name='" G_PASTE_GNOME_SHELL_GRAB_ACCELERATORS "'>"                                    \
                "<arg type='a(su)' direction='in'  name='accelerators' />"                                 \
                "<arg type='au'    direction='out' name='actions'      />"                                 \
            "</method>"                                                                                    \
            "<method name='" G_PASTE_GNOME_SHELL_UNGRAB_ACCELERATOR "'>"                                   \
                "<arg type='u' direction='in'  name='action'  />"                                          \
                "<arg type='b' direction='out' name='success' />"                                          \
            "</method>"                                                                                    \
            "<signal name='" G_PASTE_GNOME_SHELL_SIG_ACCELERATOR_ACTIVATED "'>"                            \
                "<arg name='action'    type='u' />"                                                        \
                "<arg name='deviceid'  type='u' />"                                                        \
                "<arg name='timestamp' type='u' />"                                                        \
            "</signal>"                                                                                    \
            "<property name='" G_PASTE_GNOME_SHELL_PROP_MODE "'            type='s' access='read'      />" \
            "<property name='" G_PASTE_GNOME_SHELL_PROP_OVERVIEW_ACTIVE "' type='b' access='readwrite' />" \
            "<property name='" G_PASTE_GNOME_SHELL_PROP_SHELL_VERSION "'   type='s' access='read'      />" \
        "</interface>"                                                                                     \
    "</node>"

G_DEFINE_TYPE (GPasteGnomeShellClient, g_paste_gnome_shell_client, G_TYPE_DBUS_PROXY)

enum
{
    ACCELERATOR_ACTIVATED,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

/*******************/
/* Methods / Async */
/*******************/

#define DBUS_CALL_NO_PARAM_ASYNC(method) \
    DBUS_CALL_NO_PARAM_ASYNC_BASE (GNOME_SHELL_CLIENT, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_ONE_PARAM_ASYNC(method, param_type, param_name) \
    DBUS_CALL_ONE_PARAM_ASYNC_BASE (GNOME_SHELL_CLIENT, param_type, param_name, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_ONE_PARAMV_ASYNC(method, paramv) \
    DBUS_CALL_ONE_PARAMV_ASYNC_BASE (GNOME_SHELL_CLIENT, paramv, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_TWO_PARAMS_ASYNC(method, params) \
    DBUS_CALL_TWO_PARAMS_ASYNC_BASE (GNOME_SHELL_CLIENT, params, G_PASTE_GNOME_SHELL_##method)

/****************************/
/* Methods / Async - Finish */
/****************************/

#define DBUS_ASYNC_FINISH_NO_RETURN \
    DBUS_ASYNC_FINISH_NO_RETURN_BASE (GNOME_SHELL_CLIENT)

#define DBUS_ASYNC_FINISH_RET_BOOL \
    DBUS_ASYNC_FINISH_RET_BOOL_BASE (GNOME_SHELL_CLIENT)

#define DBUS_ASYNC_FINISH_RET_UINT32 \
    DBUS_ASYNC_FINISH_RET_UINT32_BASE (GNOME_SHELL_CLIENT)

#define DBUS_ASYNC_FINISH_RET_AU \
    DBUS_ASYNC_FINISH_RET_AU_BASE (GNOME_SHELL_CLIENT)

#define DBUS_ASYNC_FINISH_RET_BS \
    DBUS_ASYNC_FINISH_RET_BS_BASE (GNOME_SHELL_CLIENT)

/******************************/
/* Methods / Sync - No return */
/******************************/

#define DBUS_CALL_NO_PARAM_NO_RETURN(method) \
    DBUS_CALL_NO_PARAM_NO_RETURN_BASE (GNOME_SHELL_CLIENT, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_ONE_PARAM_NO_RETURN(method, param_type, param_name) \
    DBUS_CALL_ONE_PARAM_NO_RETURN_BASE (GNOME_SHELL_CLIENT, param_type, param_name, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_ONE_PARAMV_NO_RETURN(method, paramv) \
    DBUS_CALL_ONE_PARAMV_NO_RETURN_BASE (GNOME_SHELL_CLIENT, paramv, G_PASTE_GNOME_SHELL_##method)

/********************************/
/* Methods / Sync - With return */
/********************************/

#define DBUS_CALL_ONE_PARAM_RET_BOOL(method, param_type, param_name) \
    DBUS_CALL_ONE_PARAM_RET_BOOL_BASE (GNOME_SHELL_CLIENT, param_type, param_name, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_ONE_PARAM_RET_BS(method, param_type, param_name) \
    DBUS_CALL_ONE_PARAM_RET_BS_BASE (GNOME_SHELL_CLIENT, param_type, param_name, G_PASTE_GNOME_SHELL_##method)

#define DBUS_CALL_ONE_PARAMV_RET_AU(method, paramv) \
    DBUS_CALL_ONE_PARAMV_RET_AU_BASE (GNOME_SHELL_CLIENT, G_PASTE_GNOME_SHELL_##method, paramv)

#define DBUS_CALL_TWO_PARAMS_RET_UINT32(method, params) \
    DBUS_CALL_TWO_PARAMS_RET_UINT32_BASE (GNOME_SHELL_CLIENT, params, G_PASTE_GNOME_SHELL_##method)

/**************/
/* Properties */
/**************/

#define DBUS_GET_BOOLEAN_PROPERTY(property) \
    DBUS_GET_BOOLEAN_PROPERTY_BASE (GNOME_SHELL_CLIENT, G_PASTE_GNOME_SHELL_PROP_##property)

#define DBUS_GET_STRING_PROPERTY(property) \
    DBUS_GET_STRING_PROPERTY_BASE (GNOME_SHELL_CLIENT, G_PASTE_GNOME_SHELL_PROP_##property)

#define DBUS_SET_BOOLEAN_PROPERTY(property, value) \
    DBUS_SET_BOOLEAN_PROPERTY_BASE (GNOME_SHELL_CLIENT, G_PASTE_GNOME_SHELL_INTERFACE_NAME, G_PASTE_GNOME_SHELL_PROP_##property, value)

static void
_g_variant_builder_add_vardict_entry (GVariantBuilder *builder,
                                      const gchar     *key,
                                      GVariant        *value)
{
    g_variant_builder_add_value (builder, g_variant_new_dict_entry (g_variant_new_string (key),
                                                                    g_variant_new_variant (value)));
}

/******************/
/* Methods / Sync */
/******************/

/**
 * g_paste_gnome_shell_client_eval_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @script: The script to eval
 * @output: (out callee-allocates) (allow-none): a plate to put the result in
 * @error: a #GError
 *
 * Evaluate a javascript script
 *
 * Returns: Whether the eval was successful or not
 */
G_PASTE_VISIBLE gboolean
g_paste_gnome_shell_client_eval_sync (GPasteGnomeShellClient *self,
                                      const gchar            *script,
                                      gchar                 **output,
                                      GError                **error)
{
    DBUS_CALL_ONE_PARAM_RET_BS (EVAL, string, script);
    if (output)
        *output = bs.s;
    return bs.b;
}

/**
 * g_paste_gnome_shell_client_focus_seach_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @error: a #GError
 *
 * Focus the search field in overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_focus_search_sync (GPasteGnomeShellClient *self,
                                              GError                **error)
{
    DBUS_CALL_NO_PARAM_NO_RETURN (FOCUS_SEARCH);
}

/**
 * g_paste_gnome_shell_client_show_osd_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @icon: (allow-none): the icon to display
 * @label: (allow-none): the text to display
 * @level: percentage to fill the bar with (-1 for none)
 * @error: a #GError
 *
 * Display something to the user
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_show_osd_sync (GPasteGnomeShellClient *self,
                                          const gchar            *icon,
                                          const gchar            *label,
                                          gint32                  level,
                                          GError                **error)
{
    GVariantBuilder builder;

    g_variant_builder_init (&builder, G_VARIANT_TYPE_VARDICT);

    if (icon)
        _g_variant_builder_add_vardict_entry (&builder, "icon", g_variant_new_string (icon));
    if (label)
        _g_variant_builder_add_vardict_entry (&builder, "label", g_variant_new_string (label));
    if (level >= 0)
        _g_variant_builder_add_vardict_entry (&builder, "level", g_variant_new_int32 (level));

    GVariant *vardict = g_variant_builder_end (&builder);

    DBUS_CALL_ONE_PARAMV_NO_RETURN (SHOW_OSD, vardict);
}

/**
 * g_paste_gnome_shell_client_focus_app_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @id: the application id
 * @error: a #GError
 *
 * Focus an app in overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_focus_app_sync (GPasteGnomeShellClient *self,
                                           const gchar            *id,
                                           GError                **error)
{
    DBUS_CALL_ONE_PARAM_NO_RETURN (FOCUS_APP, string, id);
}

/**
 * g_paste_gnome_shell_client_show_applications_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @error: a #GError
 *
 * Display the application pane in the overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_show_applications_sync (GPasteGnomeShellClient *self,
                                                   GError                **error)
{
    DBUS_CALL_NO_PARAM_NO_RETURN (SHOW_APPLICATIONS);
}

/**
 * g_paste_gnome_shell_client_grab_accelerator_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @accelerator: a #GPasteGnomeShellAccelerator instance
 * @error: a #GError
 *
 * Grab a keybinding
 *
 * Returns: the action id corresponding
 */
G_PASTE_VISIBLE guint32
g_paste_gnome_shell_client_grab_accelerator_sync (GPasteGnomeShellClient     *self,
                                                  GPasteGnomeShellAccelerator accelerator,
                                                  GError                    **error)
{
    GVariant *accel[] = {
        g_variant_new_string (accelerator.accelerator),
        g_variant_new_uint32 (accelerator.flags)
    };
    DBUS_CALL_TWO_PARAMS_RET_UINT32 (GRAB_ACCELERATOR, accel);
}

/**
 * g_paste_gnome_shell_client_grab_accelerators_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @accelerators: (array): an array of #GPasteGnomeShellAccelerator instances
 * @error: a #GError
 *
 * Grab some keybindings
 *
 * Returns: the action ids corresponding
 */
G_PASTE_VISIBLE guint32 *
g_paste_gnome_shell_client_grab_accelerators_sync (GPasteGnomeShellClient      *self,
                                                   GPasteGnomeShellAccelerator *accelerators,
                                                   GError                     **error)
{
    GVariantBuilder builder;
    guint n_accelerators = 0;

    g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

    for (GPasteGnomeShellAccelerator *accelerator = &accelerators[0]; accelerator->accelerator; accelerator = &accelerators[++n_accelerators])
    {
        g_variant_builder_open (&builder, G_VARIANT_TYPE_TUPLE);
        g_variant_builder_add_value (&builder, g_variant_new_string (accelerator->accelerator));
        g_variant_builder_add_value (&builder, g_variant_new_uint32 (accelerator->flags));
        g_variant_builder_close (&builder);
    }

    GVariant *array = g_variant_builder_end (&builder);

    DBUS_CALL_ONE_PARAMV_RET_AU (GRAB_ACCELERATORS, array);
}

/**
 * g_paste_gnome_shell_client_ungrab_accelerator_sync:
 * @self: a #GPasteGnomeShellClient instance
 * @action: the action id corresponding to the keybinding
 * @error: a #GError
 *
 * Ungrab a keybinding
 *
 * Returns: whether the ungrab was succesful or not
 */
G_PASTE_VISIBLE gboolean
g_paste_gnome_shell_client_ungrab_accelerator_sync (GPasteGnomeShellClient *self,
                                                    guint32                 action,
                                                    GError                **error)
{
    DBUS_CALL_ONE_PARAM_RET_BOOL (UNGRAB_ACCELERATOR, uint32, action);
}

/*******************/
/* Methods / Async */
/*******************/

/**
 * g_paste_gnome_shell_client_eval:
 * @self: a #GPasteGnomeShellClient instance
 * @script: The script to eval
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Evaluate a javascript script
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_eval (GPasteGnomeShellClient *self,
                                 const gchar            *script,
                                 GAsyncReadyCallback     callback,
                                 gpointer                user_data)
{
    DBUS_CALL_ONE_PARAM_ASYNC (EVAL, string, script);
}

/**
 * g_paste_gnome_shell_client_focus_seach:
 * @self: a #GPasteGnomeShellClient instance
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Focus the search field in overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_focus_search (GPasteGnomeShellClient *self,
                                         GAsyncReadyCallback     callback,
                                         gpointer                user_data)
{
    DBUS_CALL_NO_PARAM_ASYNC (FOCUS_SEARCH);
}

/**
 * g_paste_gnome_shell_client_show_osd:
 * @self: a #GPasteGnomeShellClient instance
 * @icon: (allow-none): the icon to display
 * @label: (allow-none): the text to display
 * @level: percentage to fill the bar with (-1 for none)
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Display something to the user
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_show_osd (GPasteGnomeShellClient *self,
                                     const gchar            *icon,
                                     const gchar            *label,
                                     gint32                  level,
                                     GAsyncReadyCallback     callback,
                                     gpointer                user_data)
{
    GVariantBuilder builder;

    g_variant_builder_init (&builder, G_VARIANT_TYPE_VARDICT);

    if (icon)
        _g_variant_builder_add_vardict_entry (&builder, "icon", g_variant_new_string (icon));
    if (label)
        _g_variant_builder_add_vardict_entry (&builder, "label", g_variant_new_string (label));
    if (level >= 0)
        _g_variant_builder_add_vardict_entry (&builder, "level", g_variant_new_int32 (level));

    GVariant *vardict = g_variant_builder_end (&builder);

    DBUS_CALL_ONE_PARAMV_ASYNC (SHOW_OSD, vardict);
}

/**
 * g_paste_gnome_shell_client_focus_app:
 * @self: a #GPasteGnomeShellClient instance
 * @id: the application id
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Focus an app in overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_focus_app (GPasteGnomeShellClient *self,
                                      const gchar            *id,
                                      GAsyncReadyCallback     callback,
                                      gpointer                user_data)
{
    DBUS_CALL_ONE_PARAM_ASYNC (FOCUS_APP, string, id);
}

/**
 * g_paste_gnome_shell_client_show_applications:
 * @self: a #GPasteGnomeShellClient instance
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Display the application pane in the overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_show_applications (GPasteGnomeShellClient *self,
                                              GAsyncReadyCallback     callback,
                                              gpointer                user_data)
{
    DBUS_CALL_NO_PARAM_ASYNC (SHOW_APPLICATIONS);
}

/**
 * g_paste_gnome_shell_client_grab_accelerator:
 * @self: a #GPasteGnomeShellClient instance
 * @accelerator: a #GPasteGnomeShellAccelerator instance
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Grab a keybinding
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_grab_accelerator (GPasteGnomeShellClient     *self,
                                             GPasteGnomeShellAccelerator accelerator,
                                             GAsyncReadyCallback         callback,
                                             gpointer                    user_data)
{
    GVariant *accel[] = {
        g_variant_new_string (accelerator.accelerator),
        g_variant_new_uint32 (accelerator.flags)
    };
    DBUS_CALL_TWO_PARAMS_ASYNC (GRAB_ACCELERATOR, accel);
}

/**
 * g_paste_gnome_shell_client_grab_accelerators:
 * @self: a #GPasteGnomeShellClient instance
 * @accelerators: (array): an array of #GPasteGnomeShellAccelerator instances
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Grab some keybindings
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_grab_accelerators (GPasteGnomeShellClient      *self,
                                              GPasteGnomeShellAccelerator *accelerators,
                                              GAsyncReadyCallback          callback,
                                              gpointer                     user_data)
{
    GVariantBuilder builder;
    guint n_accelerators = 0;

    g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

    for (GPasteGnomeShellAccelerator *accelerator = &accelerators[0]; accelerator->accelerator; accelerator = &accelerators[++n_accelerators])
    {
        g_variant_builder_open (&builder, G_VARIANT_TYPE_TUPLE);
        g_variant_builder_add_value (&builder, g_variant_new_string (accelerator->accelerator));
        g_variant_builder_add_value (&builder, g_variant_new_uint32 (accelerator->flags));
        g_variant_builder_close (&builder);
    }

    GVariant *array = g_variant_builder_end (&builder);

    DBUS_CALL_ONE_PARAMV_ASYNC (GRAB_ACCELERATORS, array);
}

/**
 * g_paste_gnome_shell_client_ungrab_accelerator:
 * @self: a #GPasteGnomeShellClient instance
 * @action: the action id corresponding to the keybinding
 * @callback: (allow-none): A #GAsyncReadyCallback to call when the request is satisfied or %NULL if you don't
 * care about the result of the method invocation.
 * @user_data: The data to pass to @callback.
 *
 * Ungrab a keybinding
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_ungrab_accelerator (GPasteGnomeShellClient *self,
                                               guint32                 action,
                                               GAsyncReadyCallback     callback,
                                               gpointer                user_data)
{
    DBUS_CALL_ONE_PARAM_ASYNC (UNGRAB_ACCELERATOR, uint32, action);
}

/****************************/
/* Methods / Async - Finish */
/****************************/

/**
 * g_paste_gnome_shell_client_eval_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @output: (out callee-allocates) (allow-none): a plate to put the result in
 * @error: a #GError
 *
 * Evaluate a javascript script
 *
 * Returns: Whether the eval was successful or not
 */
G_PASTE_VISIBLE gboolean
g_paste_gnome_shell_client_eval_finish (GPasteGnomeShellClient *self,
                                        GAsyncResult           *result,
                                        gchar                 **output,
                                        GError                **error)
{
    DBUS_ASYNC_FINISH_RET_BS;
    if (output)
        *output = bs.s;
    return bs.b;
}

/**
 * g_paste_gnome_shell_client_focus_seach_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @res: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Focus the search field in overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_focus_search_finish (GPasteGnomeShellClient *self,
                                                GAsyncResult           *result,
                                                GError                **error)
{
    DBUS_ASYNC_FINISH_NO_RETURN;
}

/**
 * g_paste_gnome_shell_client_show_osd_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Display something to the user
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_show_osd_finish (GPasteGnomeShellClient *self,
                                            GAsyncResult           *result,
                                            GError                **error)
{
    DBUS_ASYNC_FINISH_NO_RETURN;
}

/**
 * g_paste_gnome_shell_client_focus_app_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Focus an app in overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_focus_app_finish (GPasteGnomeShellClient *self,
                                             GAsyncResult           *result,
                                             GError                **error)
{
    DBUS_ASYNC_FINISH_NO_RETURN;
}

/**
 * g_paste_gnome_shell_client_show_applications_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Display the application pane in the overview
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_show_applications_finish (GPasteGnomeShellClient *self,
                                                     GAsyncResult           *result,
                                                     GError                **error)
{
    DBUS_ASYNC_FINISH_NO_RETURN;
}

/**
 * g_paste_gnome_shell_client_grab_accelerator_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Grab a keybinding
 *
 * Returns: the action id corresultponding
 */
G_PASTE_VISIBLE guint32
g_paste_gnome_shell_client_grab_accelerator_finish (GPasteGnomeShellClient *self,
                                                    GAsyncResult           *result,
                                                    GError                **error)
{
    DBUS_ASYNC_FINISH_RET_UINT32;
}

/**
 * g_paste_gnome_shell_client_grab_accelerators_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Grab some keybindings
 *
 * Returns: the action ids corresultponding
 */
G_PASTE_VISIBLE guint32 *
g_paste_gnome_shell_client_grab_accelerators_finish (GPasteGnomeShellClient *self,
                                                     GAsyncResult           *result,
                                                     GError                **error)
{
    DBUS_ASYNC_FINISH_RET_AU;
}

/**
 * g_paste_gnome_shell_client_ungrab_accelerator_finish:
 * @self: a #GPasteGnomeShellClient instance
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback passed to the async call.
 * @error: a #GError
 *
 * Ungrab a keybinding
 *
 * Returns: whether the ungrab was succesful or not
 */
G_PASTE_VISIBLE gboolean
g_paste_gnome_shell_client_ungrab_accelerator_finish (GPasteGnomeShellClient *self,
                                                      GAsyncResult           *result,
                                                      GError                **error)
{
    DBUS_ASYNC_FINISH_RET_BOOL;
}

/**************/
/* Properties */
/**************/

/**
 * g_paste_gnome_shell_client_get_mode:
 * @self: a #GPasteGnomeShellClient instance
 *
 * Get the mode gnome-shell is ran as
 *
 * Returns: the "Mode" property
 */
G_PASTE_VISIBLE const gchar *
g_paste_gnome_shell_client_get_mode (GPasteGnomeShellClient *self)
{
    DBUS_GET_STRING_PROPERTY (MODE);
}

/**
 * g_paste_gnome_shell_client_overview_is_active:
 * @self: a #GPasteGnomeShellClient instance
 *
 * Whether the overview is active or not
 *
 * Returns: the "OverviewActive" property
 */
G_PASTE_VISIBLE gboolean
g_paste_gnome_shell_client_overview_is_active (GPasteGnomeShellClient *self)
{
    DBUS_GET_BOOLEAN_PROPERTY (OVERVIEW_ACTIVE);
}

/**
 * g_paste_gnome_shell_client_get_shell_version:
 * @self: a #GPasteGnomeShellClient instance
 *
 * Get the shell version
 *
 * Returns: the "ShellVersion" property
 */
G_PASTE_VISIBLE const gchar *
g_paste_gnome_shell_client_get_shell_version (GPasteGnomeShellClient *self)
{
    DBUS_GET_STRING_PROPERTY (SHELL_VERSION);
}

/**
 * g_paste_gnome_shell_client_overview_set_active:
 * @self: a #GPasteGnomeShellClient instance
 * @value: the active state
 * @error: a #GError
 *
 * Set whether the overview is active or not
 *
 * Returns: Whether the "OverviewActive" property has been set or not
 */
G_PASTE_VISIBLE gboolean
g_paste_gnome_shell_client_overview_set_active (GPasteGnomeShellClient *self,
                                                gboolean                value,
                                                GError                **error)
{
    DBUS_SET_BOOLEAN_PROPERTY (OVERVIEW_ACTIVE, value);
}

static void
g_paste_gnome_shell_client_g_signal (GDBusProxy  *proxy,
                                     const gchar *sender_name G_GNUC_UNUSED,
                                     const gchar *signal_name,
                                     GVariant    *parameters)
{
    GPasteGnomeShellClient *self = G_PASTE_GNOME_SHELL_CLIENT (proxy);

    if (!g_strcmp0 (signal_name, G_PASTE_GNOME_SHELL_SIG_ACCELERATOR_ACTIVATED))
    {
        GVariantIter params_iter;
        g_variant_iter_init (&params_iter, parameters);
        G_PASTE_CLEANUP_VARIANT_UNREF GVariant *action    = g_variant_iter_next_value (&params_iter);
        G_PASTE_CLEANUP_VARIANT_UNREF GVariant *deviceid  = g_variant_iter_next_value (&params_iter);
        G_PASTE_CLEANUP_VARIANT_UNREF GVariant *timestamp = g_variant_iter_next_value (&params_iter);
        g_signal_emit (self,
                       signals[ACCELERATOR_ACTIVATED],
                       0, /* detail */
                       g_variant_get_uint32 (action),
                       g_variant_get_uint32 (deviceid),
                       g_variant_get_uint32 (timestamp),
                       NULL);
    }
}

static void
g_paste_gnome_shell_client_class_init (GPasteGnomeShellClientClass *klass G_GNUC_UNUSED)
{
    G_DBUS_PROXY_CLASS (klass)->g_signal = g_paste_gnome_shell_client_g_signal;

    signals[ACCELERATOR_ACTIVATED] = g_signal_new ("accelerator-activated",
                                                   G_PASTE_TYPE_GNOME_SHELL_CLIENT,
                                                   G_SIGNAL_RUN_LAST,
                                                   0, /* class offset */
                                                   NULL, /* accumulator */
                                                   NULL, /* accumulator data */
                                                   g_cclosure_marshal_generic,
                                                   G_TYPE_NONE,
                                                   3,
                                                   G_TYPE_UINT,
                                                   G_TYPE_UINT,
                                                   G_TYPE_UINT);
}

static void
g_paste_gnome_shell_client_init (GPasteGnomeShellClient *self)
{
    GDBusProxy *proxy = G_DBUS_PROXY (self);

    G_PASTE_CLEANUP_NODE_INFO_UNREF GDBusNodeInfo *gnome_shell_dbus_info = g_dbus_node_info_new_for_xml (G_PASTE_GNOME_SHELL_INTERFACE,
                                                                                                         NULL); /* Error */
    g_dbus_proxy_set_interface_info (proxy, gnome_shell_dbus_info->interfaces[0]);
}

/**
 * g_paste_gnome_shell_client_new_sync:
 * @error: Return location for error or %NULL.
 *
 * Create a new instance of #GPasteGnomeShellClient
 *
 * Returns: a newly allocated #GPasteGnomeShellClient
 *          free it with g_object_unref
 */
G_PASTE_VISIBLE GPasteGnomeShellClient *
g_paste_gnome_shell_client_new_sync (GError **error)
{
    CUSTOM_PROXY_NEW (GNOME_SHELL_CLIENT, GNOME_SHELL);
}

/**
 * g_paste_gnome_shell_client_new:
 * @callback: Callback function to invoke when the proxy is ready.
 * @user_data: User data to pass to @callback.
 *
 * Create a new instance of #GPasteGnomeShellClient
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_gnome_shell_client_new (GAsyncReadyCallback callback,
                                gpointer            user_data)
{
    CUSTOM_PROXY_NEW_ASYNC (GNOME_SHELL_CLIENT, GNOME_SHELL);
}

/**
 * g_paste_gnome_shell_client_new_finsh:
 * @result: A #GAsyncResult obtained from the #GAsyncReadyCallback function passed to the async ctor.
 * @error: Return location for error or %NULL.
 *
 * Create a new instance of #GPasteGnomeShellClient
 *
 * Returns: a newly allocated #GPasteGnomeShellClient
 *          free it with g_object_unref
 */
G_PASTE_VISIBLE GPasteGnomeShellClient *
g_paste_gnome_shell_client_new_finish (GAsyncResult *result,
                                       GError      **error)
{
    CUSTOM_PROXY_NEW_FINISH (GNOME_SHELL_CLIENT);
}
