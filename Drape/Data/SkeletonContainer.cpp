#include "SkeletonContainer.h"
#include "SkeletonUtility.h"


SkeletonContainer::SkeletonContainer(void)
{
}


SkeletonContainer::~SkeletonContainer(void)
{
}

bool SkeletonContainer::addSkeletonFromMesh( const Mesh& mesh )
{
	mSkeletonList.push_back(Skeleton());
	Skeleton &skeleton = mSkeletonList[mSkeletonList.size()-1];
	SkeletonExtractor extractor;
	extractor.extract(mesh, skeleton);
	SkeletonUtility skeletonUtility;
	skeletonUtility.write(skeleton,"skeleton.txt");
	skeleton.findNeck();
	return true;
}

Skeleton& SkeletonContainer::getSkeletonRef( int i )
{
	assert(i < size());
	return mSkeletonList[i];
}

int SkeletonContainer::size()
{
	return mSkeletonList.size();
}

SkeletonContainer globalSkeletonContainer;