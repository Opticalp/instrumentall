/**
 * @file	src/core/TaskNotification.h
 * @date	June 2016
 * @author	Applied Informatics Software Engineering GmbH and contributors
 *
 * Original Poco::Task code checked out from https://github.com/pocoproject/poco
 * commit d6ebf591 (May 2016)
 */

//
// TaskNotification.h
//
// $Id: //poco/1.4/Foundation/include/Poco/TaskNotification.h#1 $
//
// Library: Foundation
// Package: Tasks
// Module:  Tasks
//
// Definition of the TaskNotification class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef CUSTOM_Foundation_TaskNotification_INCLUDED
#define CUSTOM_Foundation_TaskNotification_INCLUDED


#include "Poco/Notification.h"
#include "Poco/Exception.h"

class MergeableTask;


class TaskNotification: public Poco::Notification
	/// Base class for TaskManager notifications.
{
public:
	TaskNotification(MergeableTask* pTask);
		/// Creates the TaskNotification.

	MergeableTask* task() const;
		/// Returns the subject of the notification.

protected:
	virtual ~TaskNotification();
		/// Destroys the TaskNotification.

private:
	MergeableTask* _pTask;
};


class TaskStartedNotification: public TaskNotification
	/// This notification is posted by the TaskManager for
	/// every task that has been started.
{
public:
	TaskStartedNotification(MergeableTask* pTask);
	
protected:
	~TaskStartedNotification();
};


class TaskCancelledNotification: public TaskNotification
	/// This notification is posted by the TaskManager for
	/// every task that has been cancelled.
{
public:
	TaskCancelledNotification(MergeableTask* pTask);

protected:
	~TaskCancelledNotification();
};


class TaskFinishedNotification: public TaskNotification
	/// This notification is posted by the TaskManager for
	/// every task that has finished.
{
public:
	TaskFinishedNotification(MergeableTask* pTask);

protected:
	~TaskFinishedNotification();
};


class TaskEnslavedNotification: public TaskNotification
	/// This notification is posted by the TaskManager for
	/// every task that has been enslaved.
{
public:
	TaskEnslavedNotification(MergeableTask* pTask, MergeableTask* enslaved);

	MergeableTask* slave() const;

protected:
	~TaskEnslavedNotification();

private:
	MergeableTask* mSlave;
};


class TaskFailedNotification: public TaskNotification
	/// This notification is posted by the TaskManager for
	/// every task that has failed with an exception.
{
public:
	TaskFailedNotification(MergeableTask* pTask, const Poco::Exception& exc);

	const Poco::Exception& reason() const;

protected:
	~TaskFailedNotification();
	
private:
	Poco::Exception* _pException;
};


class TaskFailedOnCancellationNotification: public TaskNotification
    /// This notification is posted by the TaskManager for
    /// every task that has failed after a cancellation.
{
public:
    TaskFailedOnCancellationNotification(MergeableTask* pTask, const Poco::Exception& exc);

    const Poco::Exception& reason() const;

protected:
    ~TaskFailedOnCancellationNotification();

private:
    Poco::Exception* _pException;
};


class TaskProgressNotification: public TaskNotification
	/// This notification is posted by the TaskManager for
	/// a task when its progress changes.
{
public:
	TaskProgressNotification(MergeableTask* pTask, float progress);

	float progress() const;

protected:
	~TaskProgressNotification();
	
private:
	float _progress;
};


//template <class C>
//class TaskCustomNotification: public TaskNotification
//	/// This is a template for "custom" notification.
//	/// Unlike other notifications, this notification
//	/// is instantiated and posted by the task itself.
//	/// The purpose is to provide generic notification
//	/// mechanism between the task and its observer(s).
//{
//public:
//	TaskCustomNotification(MergeableTask* pTask, const C& rCustom):
//		TaskNotification(pTask),
//		_custom(rCustom)
//	{
//	}
//
//	const C& custom() const
//	{
//		return _custom;
//	}
//
//protected:
//	~TaskCustomNotification(){};
//
//private:
//	C _custom;
//};


//
// inlines
//
inline MergeableTask* TaskNotification::task() const
{
	return _pTask;
}


inline const Poco::Exception& TaskFailedNotification::reason() const
{
	return *_pException;
}

inline const Poco::Exception& TaskFailedOnCancellationNotification::reason() const
{
    return *_pException;
}


inline MergeableTask* TaskEnslavedNotification::slave() const
{
	return mSlave;
}


inline float TaskProgressNotification::progress() const
{
	return _progress;
}


#endif // CUSTOM_Foundation_TaskNotification_INCLUDED
