#----------------------------------------------------------------------------------
# File:        native_globe/jni/Android.mk
# SDK Version: Android NVIDIA samples 4 
# Email:       tegradev@nvidia.com
# Site:        http://developer.nvidia.com/
#
# Copyright (c) 2007-2012, NVIDIA CORPORATION.  All rights reserved.
#
# TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
# *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
# OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
# BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
# WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
# BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
# ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
# BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
#
#
#----------------------------------------------------------------------------------
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := native_globe

#LOCAL_SRC_FILES := $(wildcard *.cpp)
#LOCAL_SRC_FILES += $(wildcard *.c)
#LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/bullet/*.cpp)
#LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/bullet/*.c)

libBullet_la_SOURCES := \
		bullet/bChunk.cpp \
		bullet/bChunk.h \
		bullet/bCommon.h \
		bullet/bDefines.h \
		bullet/bDNA.cpp \
		bullet/bDNA.h \
		bullet/bFile.cpp \
		bullet/bFile.h \
		bullet/btAabbUtil2.h \
		bullet/btActionInterface.h \
		bullet/btActivatingCollisionAlgorithm.cpp \
		bullet/btActivatingCollisionAlgorithm.h \
		bullet/btAlignedAllocator.cpp \
		bullet/btAlignedAllocator.h \
		bullet/btAlignedObjectArray.h \
		bullet/btAxisSweep3.cpp \
		bullet/btAxisSweep3.h \
		bullet/btBox2dBox2dCollisionAlgorithm.cpp \
		bullet/btBox2dBox2dCollisionAlgorithm.h \
		bullet/btBox2dShape.cpp \
		bullet/btBox2dShape.h \
		bullet/btBoxBoxCollisionAlgorithm.cpp \
		bullet/btBoxBoxCollisionAlgorithm.h \
		bullet/btBoxBoxDetector.cpp \
		bullet/btBoxBoxDetector.h \
		bullet/btBoxCollision.h \
		bullet/btBoxShape.cpp \
		bullet/btBoxShape.h \
		bullet/btBroadphaseInterface.h \
		bullet/btBroadphaseProxy.cpp \
		bullet/btBroadphaseProxy.h \
		bullet/btBulletCollisionCommon.h \
		bullet/btBulletDynamicsCommon.h \
		bullet/btBulletFile.cpp \
		bullet/btBulletFile.h \
		bullet/btBulletWorldImporter.cpp \
		bullet/btBulletWorldImporter.h \
		bullet/btBvhTriangleMeshShape.cpp \
		bullet/btBvhTriangleMeshShape.h \
		bullet/btCapsuleShape.cpp \
		bullet/btCapsuleShape.h \
		bullet/btCharacterControllerInterface.h \
		bullet/btClipPolygon.h \
		bullet/btCollisionAlgorithm.cpp \
		bullet/btCollisionAlgorithm.h \
		bullet/btCollisionConfiguration.h \
		bullet/btCollisionCreateFunc.h \
		bullet/btCollisionDispatcher.cpp \
		bullet/btCollisionDispatcher.h \
		bullet/btCollisionMargin.h \
		bullet/btCollisionObject.cpp \
		bullet/btCollisionObject.h \
		bullet/btCollisionShape.cpp \
		bullet/btCollisionShape.h \
		bullet/btCollisionWorld.cpp \
		bullet/btCollisionWorld.h \
		bullet/btCompoundCollisionAlgorithm.cpp \
		bullet/btCompoundCollisionAlgorithm.h \
		bullet/btCompoundShape.cpp \
		bullet/btCompoundShape.h \
		bullet/btConcaveShape.cpp \
		bullet/btConcaveShape.h \
		bullet/btConeShape.cpp \
		bullet/btConeShape.h \
		bullet/btConeTwistConstraint.cpp \
		bullet/btConeTwistConstraint.h \
		bullet/btConstraintSolver.h \
		bullet/btContactConstraint.cpp \
		bullet/btContactConstraint.h \
		bullet/btContactProcessing.cpp \
		bullet/btContactProcessing.h \
		bullet/btContactSolverInfo.h \
		bullet/btContinuousConvexCollision.cpp \
		bullet/btContinuousConvexCollision.h \
		bullet/btContinuousDynamicsWorld.cpp \
		bullet/btContinuousDynamicsWorld.h \
		bullet/btConvex2dConvex2dAlgorithm.cpp \
		bullet/btConvex2dConvex2dAlgorithm.h \
		bullet/btConvex2dShape.cpp \
		bullet/btConvex2dShape.h \
		bullet/btConvexCast.cpp \
		bullet/btConvexCast.h \
		bullet/btConvexConcaveCollisionAlgorithm.cpp \
		bullet/btConvexConcaveCollisionAlgorithm.h \
		bullet/btConvexConvexAlgorithm.cpp \
		bullet/btConvexConvexAlgorithm.h \
		bullet/btConvexHull.cpp \
		bullet/btConvexHull.h \
		bullet/btConvexHullComputer.cpp \
		bullet/btConvexHullComputer.h \
		bullet/btConvexHullShape.cpp \
		bullet/btConvexHullShape.h \
		bullet/btConvexInternalShape.cpp \
		bullet/btConvexInternalShape.h \
		bullet/btConvexPenetrationDepthSolver.h \
		bullet/btConvexPlaneCollisionAlgorithm.cpp \
		bullet/btConvexPlaneCollisionAlgorithm.h \
		bullet/btConvexPointCloudShape.cpp \
		bullet/btConvexPointCloudShape.h \
		bullet/btConvexPolyhedron.cpp \
		bullet/btConvexPolyhedron.h \
		bullet/btConvexShape.cpp \
		bullet/btConvexShape.h \
		bullet/btConvexTriangleMeshShape.cpp \
		bullet/btConvexTriangleMeshShape.h \
		bullet/btCylinderShape.cpp \
		bullet/btCylinderShape.h \
		bullet/btDbvt.cpp \
		bullet/btDbvt.h \
		bullet/btDbvtBroadphase.cpp \
		bullet/btDbvtBroadphase.h \
		bullet/btDefaultCollisionConfiguration.cpp \
		bullet/btDefaultCollisionConfiguration.h \
		bullet/btDefaultMotionState.h \
		bullet/btDefaultSoftBodySolver.cpp \
		bullet/btDefaultSoftBodySolver.h \
		bullet/btDiscreteCollisionDetectorInterface.h \
		bullet/btDiscreteDynamicsWorld.cpp \
		bullet/btDiscreteDynamicsWorld.h \
		bullet/btDispatcher.cpp \
		bullet/btDispatcher.h \
		bullet/btDynamicsWorld.h \
		bullet/btEmptyCollisionAlgorithm.cpp \
		bullet/btEmptyCollisionAlgorithm.h \
		bullet/btEmptyShape.cpp \
		bullet/btEmptyShape.h \
		bullet/btGeneric6DofConstraint.cpp \
		bullet/btGeneric6DofConstraint.h \
		bullet/btGeneric6DofSpringConstraint.cpp \
		bullet/btGeneric6DofSpringConstraint.h \
		bullet/btGenericPoolAllocator.cpp \
		bullet/btGenericPoolAllocator.h \
		bullet/btGeometryOperations.h \
		bullet/btGeometryUtil.cpp \
		bullet/btGeometryUtil.h \
		bullet/btGhostObject.cpp \
		bullet/btGhostObject.h \
		bullet/btGImpactBvh.cpp \
		bullet/btGImpactBvh.h \
		bullet/btGImpactCollisionAlgorithm.cpp \
		bullet/btGImpactCollisionAlgorithm.h \
		bullet/btGImpactMassUtil.h \
		bullet/btGImpactQuantizedBvh.cpp \
		bullet/btGImpactQuantizedBvh.h \
		bullet/btGImpactShape.cpp \
		bullet/btGImpactShape.h \
		bullet/btGjkConvexCast.cpp \
		bullet/btGjkConvexCast.h \
		bullet/btGjkEpa2.cpp \
		bullet/btGjkEpa2.h \
		bullet/btGjkEpaPenetrationDepthSolver.cpp \
		bullet/btGjkEpaPenetrationDepthSolver.h \
		bullet/btGjkPairDetector.cpp \
		bullet/btGjkPairDetector.h \
		bullet/btHashMap.h \
		bullet/btHeightfieldTerrainShape.cpp \
		bullet/btHeightfieldTerrainShape.h \
		bullet/btHinge2Constraint.cpp \
		bullet/btHinge2Constraint.h \
		bullet/btHingeConstraint.cpp \
		bullet/btHingeConstraint.h \
		bullet/btIDebugDraw.h \
		bullet/btInternalEdgeUtility.cpp \
		bullet/btInternalEdgeUtility.h \
		bullet/btJacobianEntry.h \
		bullet/btKinematicCharacterController.cpp \
		bullet/btKinematicCharacterController.h \
		bullet/btList.h \
		bullet/btManifoldPoint.h \
		bullet/btManifoldResult.cpp \
		bullet/btManifoldResult.h \
		bullet/btMaterial.h \
		bullet/btMatrix3x3.h \
		bullet/btMinkowskiPenetrationDepthSolver.cpp \
		bullet/btMinkowskiPenetrationDepthSolver.h \
		bullet/btMinkowskiSumShape.cpp \
		bullet/btMinkowskiSumShape.h \
		bullet/btMinMax.h \
		bullet/btMotionState.h \
		bullet/btMultimaterialTriangleMeshShape.cpp \
		bullet/btMultimaterialTriangleMeshShape.h \
		bullet/btMultiSapBroadphase.cpp \
		bullet/btMultiSapBroadphase.h \
		bullet/btMultiSphereShape.cpp \
		bullet/btMultiSphereShape.h \
		bullet/btOptimizedBvh.cpp \
		bullet/btOptimizedBvh.h \
		bullet/btOverlappingPairCache.cpp \
		bullet/btOverlappingPairCache.h \
		bullet/btOverlappingPairCallback.h \
		bullet/btPersistentManifold.cpp \
		bullet/btPersistentManifold.h \
		bullet/btPoint2PointConstraint.cpp \
		bullet/btPoint2PointConstraint.h \
		bullet/btPointCollector.h \
		bullet/btPolyhedralContactClipping.cpp \
		bullet/btPolyhedralContactClipping.h \
		bullet/btPolyhedralConvexShape.cpp \
		bullet/btPolyhedralConvexShape.h \
		bullet/btPoolAllocator.h \
		bullet/btQuadWord.h \
		bullet/btQuantization.h \
		bullet/btQuantizedBvh.cpp \
		bullet/btQuantizedBvh.h \
		bullet/btQuaternion.h \
		bullet/btQuickprof.cpp \
		bullet/btQuickprof.h \
		bullet/btRandom.h \
		bullet/btRaycastCallback.cpp \
		bullet/btRaycastCallback.h \
		bullet/btRaycastVehicle.cpp \
		bullet/btRaycastVehicle.h \
		bullet/btRigidBody.cpp \
		bullet/btRigidBody.h \
		bullet/btScalar.h \
		bullet/btScaledBvhTriangleMeshShape.cpp \
		bullet/btScaledBvhTriangleMeshShape.h \
		bullet/btSequentialImpulseConstraintSolver.cpp \
		bullet/btSequentialImpulseConstraintSolver.h \
		bullet/btSerializer.cpp \
		bullet/btSerializer.h \
		bullet/btShapeHull.cpp \
		bullet/btShapeHull.h \
		bullet/btSimpleBroadphase.cpp \
		bullet/btSimpleBroadphase.h \
		bullet/btSimpleDynamicsWorld.cpp \
		bullet/btSimpleDynamicsWorld.h \
		bullet/btSimplexSolverInterface.h \
		bullet/btSimulationIslandManager.cpp \
		bullet/btSimulationIslandManager.h \
		bullet/btSliderConstraint.cpp \
		bullet/btSliderConstraint.h \
		bullet/btSoftBody.cpp \
		bullet/btSoftBody.h \
		bullet/btSoftBodyConcaveCollisionAlgorithm.cpp \
		bullet/btSoftBodyConcaveCollisionAlgorithm.h \
		bullet/btSoftBodyData.h \
		bullet/btSoftBodyHelpers.cpp \
		bullet/btSoftBodyHelpers.h \
		bullet/btSoftBodyInternals.h \
		bullet/btSoftBodyRigidBodyCollisionConfiguration.cpp \
		bullet/btSoftBodyRigidBodyCollisionConfiguration.h \
		bullet/btSoftBodySolvers.h \
		bullet/btSoftBodySolverVertexBuffer.h \
		bullet/btSoftRigidCollisionAlgorithm.cpp \
		bullet/btSoftRigidCollisionAlgorithm.h \
		bullet/btSoftRigidDynamicsWorld.cpp \
		bullet/btSoftRigidDynamicsWorld.h \
		bullet/btSoftSoftCollisionAlgorithm.cpp \
		bullet/btSoftSoftCollisionAlgorithm.h \
		bullet/btSolve2LinearConstraint.cpp \
		bullet/btSolve2LinearConstraint.h \
		bullet/btSolverBody.h \
		bullet/btSolverConstraint.h \
		bullet/btSparseSDF.h \
		bullet/btSphereBoxCollisionAlgorithm.cpp \
		bullet/btSphereBoxCollisionAlgorithm.h \
		bullet/btSphereShape.cpp \
		bullet/btSphereShape.h \
		bullet/btSphereSphereCollisionAlgorithm.cpp \
		bullet/btSphereSphereCollisionAlgorithm.h \
		bullet/btSphereTriangleCollisionAlgorithm.cpp \
		bullet/btSphereTriangleCollisionAlgorithm.h \
		bullet/btStackAlloc.h \
		bullet/btStaticPlaneShape.cpp \
		bullet/btStaticPlaneShape.h \
		bullet/btStridingMeshInterface.cpp \
		bullet/btStridingMeshInterface.h \
		bullet/btSubSimplexConvexCast.cpp \
		bullet/btSubSimplexConvexCast.h \
		bullet/btTetrahedronShape.cpp \
		bullet/btTetrahedronShape.h \
		bullet/btTransform.h \
		bullet/btTransformUtil.h \
		bullet/btTriangleBuffer.cpp \
		bullet/btTriangleBuffer.h \
		bullet/btTriangleCallback.cpp \
		bullet/btTriangleCallback.h \
		bullet/btTriangleIndexVertexArray.cpp \
		bullet/btTriangleIndexVertexArray.h \
		bullet/btTriangleIndexVertexMaterialArray.cpp \
		bullet/btTriangleIndexVertexMaterialArray.h \
		bullet/btTriangleInfoMap.h \
		bullet/btTriangleMesh.cpp \
		bullet/btTriangleMesh.h \
		bullet/btTriangleMeshShape.cpp \
		bullet/btTriangleMeshShape.h \
		bullet/btTriangleShape.h \
		bullet/btTriangleShapeEx.cpp \
		bullet/btTriangleShapeEx.h \
		bullet/btTypedConstraint.cpp \
		bullet/btTypedConstraint.h \
		bullet/btUniformScalingShape.cpp \
		bullet/btUniformScalingShape.h \
		bullet/btUnionFind.cpp \
		bullet/btUnionFind.h \
		bullet/btUniversalConstraint.cpp \
		bullet/btUniversalConstraint.h \
		bullet/btVector3.h \
		bullet/btVehicleRaycaster.h \
		bullet/btVoronoiSimplexSolver.cpp \
		bullet/btVoronoiSimplexSolver.h \
		bullet/btWheelInfo.cpp \
		bullet/btWheelInfo.h \
		bullet/bullet.h \
		bullet/gim_array.h \
		bullet/gim_basic_geometry_operations.h \
		bullet/gim_bitset.h \
		bullet/gim_box_collision.h \
		bullet/gim_box_set.cpp \
		bullet/gim_box_set.h \
		bullet/gim_clip_polygon.h \
		bullet/gim_contact.cpp \
		bullet/gim_contact.h \
		bullet/gim_geom_types.h \
		bullet/gim_geometry.h \
		bullet/gim_hash_table.h \
		bullet/gim_linear_math.h \
		bullet/gim_math.h \
		bullet/gim_memory.cpp \
		bullet/gim_memory.h \
		bullet/gim_radixsort.h \
		bullet/gim_tri_collision.cpp \
		bullet/gim_tri_collision.h \
		bullet/SphereTriangleDetector.cpp \
		bullet/SphereTriangleDetector.h 



LOCAL_SRC_FILES := $(wildcard *.cpp)
LOCAL_SRC_FILES += $(wildcard *.c)
LOCAL_SRC_FILES += \
		$(libBullet_la_SOURCES) \


LOCAL_ARM_MODE   := arm

LOCAL_C_INCLUDES := D:/NVSAMPLES/nvidia_tegra_android_samples_preview/libs/jni/ \
		$(LOCAL_PATH)/bullet #../../../libs/jni/
LOCAL_LDLIBS :=  -lstdc++ -lc -lm -llog -landroid -ldl -lGLESv2 -lEGL -lOpenSLES
LOCAL_STATIC_LIBRARIES := nv_and_util nv_egl_util nv_bitfont nv_math nv_glesutil nv_hhdds nv_log nv_shader nv_file nv_thread

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, D:/NVSAMPLES/nvidia_tegra_android_samples_preview/libs/jni) #../../../libs/jni)

$(call import-module,nv_and_util)
$(call import-module,nv_egl_util)
$(call import-module,nv_bitfont)
$(call import-module,nv_math)
$(call import-module,nv_glesutil)
$(call import-module,nv_hhdds)
$(call import-module,nv_log)
$(call import-module,nv_shader)
$(call import-module,nv_file)
$(call import-module,nv_thread)
