/**
 * \file
 * Copyright 2015 Xamarin Inc
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
#ifndef __MONO_METADATA_ASSEMBLY_INTERNALS_H__
#define __MONO_METADATA_ASSEMBLY_INTERNALS_H__

#include <glib.h>

#include <mono/metadata/assembly.h>
#include <mono/metadata/metadata-internals.h>

#ifndef ENABLE_NETCORE
#define MONO_ASSEMBLY_CORLIB_NAME "mscorlib"
#else
#define MONO_ASSEMBLY_CORLIB_NAME "System.Private.CoreLib"
#endif

/* Flag bits for mono_assembly_names_equal_flags (). */
typedef enum {
	/* Default comparison: all fields must match */
	MONO_ANAME_EQ_NONE = 0x0,
	/* Don't compare public key token */
	MONO_ANAME_EQ_IGNORE_PUBKEY = 0x1,
	/* Don't compare the versions */
	MONO_ANAME_EQ_IGNORE_VERSION = 0x2,
	/* When comparing simple names, ignore case differences */
	MONO_ANAME_EQ_IGNORE_CASE = 0x4,

	MONO_ANAME_EQ_MASK = 0x7
} MonoAssemblyNameEqFlags;

void
mono_assembly_name_free_internal (MonoAssemblyName *aname);

gboolean
mono_assembly_names_equal_flags (MonoAssemblyName *l, MonoAssemblyName *r, MonoAssemblyNameEqFlags flags);

gboolean
mono_assembly_get_assemblyref_checked (MonoImage *image, int index, MonoAssemblyName *aname, MonoError *error);

MONO_API MonoImage*    mono_assembly_load_module_checked (MonoAssembly *assembly, uint32_t idx, MonoError *error);

MonoAssembly* mono_assembly_load_with_partial_name_internal (const char *name, MonoImageOpenStatus *status);


typedef gboolean (*MonoAssemblyAsmCtxFromPathFunc) (const char *absfname, MonoAssembly *requesting_assembly, gpointer user_data, MonoAssemblyContextKind *out_asmctx);

void mono_install_assembly_asmctx_from_path_hook (MonoAssemblyAsmCtxFromPathFunc func, gpointer user_data);

typedef MonoAssembly * (*MonoAssemblyPreLoadFuncV2) (MonoAssemblyLoadContext *alc, MonoAssemblyName *aname, char **assemblies_path, gboolean refonly, gpointer user_data, MonoError *error);

void mono_install_assembly_preload_hook_v2 (MonoAssemblyPreLoadFuncV2 func, void *user_data, gboolean refonly);

/* If predicate returns true assembly should be loaded, if false ignore it. */
typedef gboolean (*MonoAssemblyCandidatePredicate)(MonoAssembly *, gpointer);

typedef struct MonoAssemblyLoadRequest {
	/* Assembly Load context that is requesting an assembly. */
	MonoAssemblyContextKind asmctx;
	MonoAssemblyLoadContext *alc;
	/* Predicate to apply to candidate assemblies. Optional. */
	MonoAssemblyCandidatePredicate predicate;
	/* user_data for predicate. Optional. */
	gpointer predicate_ud;
} MonoAssemblyLoadRequest;

typedef struct MonoAssemblyOpenRequest {
	MonoAssemblyLoadRequest request;
	/* Assembly that is requesting the wanted assembly. Optional. */
	MonoAssembly *requesting_assembly;
} MonoAssemblyOpenRequest;

typedef struct MonoAssemblyByNameRequest {
	MonoAssemblyLoadRequest request;
	/* Assembly that is requesting the wanted assembly name. Optional.
	 * If no_postload_search is TRUE, requesting_assembly is not used.
	 */
	MonoAssembly *requesting_assembly;
	/* basedir to probe for the wanted assembly name.  Optional. */
	const char *basedir;
	gboolean no_postload_search; /* FALSE is usual */
	/* FIXME: predicate unused? */
} MonoAssemblyByNameRequest;

void                   mono_assembly_request_prepare (MonoAssemblyLoadRequest *req,
						      size_t req_size,
						      MonoAssemblyContextKind asmctx);

MonoAssembly*          mono_assembly_request_open (const char *filename,
						     const MonoAssemblyOpenRequest *req,
						     MonoImageOpenStatus *status);

MonoAssembly*          mono_assembly_request_load_from (MonoImage *image, const char *fname,
							const MonoAssemblyLoadRequest *req,
							MonoImageOpenStatus *status);

MonoAssembly*          mono_assembly_request_byname (MonoAssemblyName *aname,
						     const MonoAssemblyByNameRequest *req,
						     MonoImageOpenStatus *status);

/* MonoAssemblyCandidatePredicate that compares the assembly name (name, version,
 * culture, public key token) of the candidate with the wanted name, if the
 * wanted name has a public key token (if not present, always return true).
 * Pass the wanted MonoAssemblyName* as the user_data.
 */
gboolean
mono_assembly_candidate_predicate_sn_same_name (MonoAssembly *candidate, gpointer wanted_name);

MonoAssembly*
mono_assembly_binding_applies_to_image (MonoImage* image, MonoImageOpenStatus *status);

MonoAssembly*
mono_assembly_load_from_assemblies_path (gchar **assemblies_path, MonoAssemblyName *aname, MonoAssemblyContextKind asmctx);

MONO_PROFILER_API MonoAssemblyName*
mono_assembly_get_name_internal (MonoAssembly *assembly);

MONO_PROFILER_API MonoImage*
mono_assembly_get_image_internal (MonoAssembly *assembly);

#endif /* __MONO_METADATA_ASSEMBLY_INTERNALS_H__ */
