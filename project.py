import os
import sys

from pyqtbuild import PyQtBindings, PyQtProject
from sipbuild import Option

class PyQtAds(PyQtProject):
	def __init__(self):
		""" Initialise the project. """

		super().__init__()

		self.bindings_factories = [ads]

class ads(PyQtBindings):
	def __init__(self, project):
		""" Initialise the bindings. """

		super().__init__(project, 'ads')
		
	def get_options(self):
		"""Our custom options that a user can pass to sip-build."""
		options = super().get_options()
		options += [
		    Option('ads_incdir',
				   help='the directory containing the ads header file',
				   metavar='DIR'),
		    Option('ads_libdir',
				   help='the directory containing the ads library',
				   metavar='DIR'),
		    Option('ads_lib',
				   help='the ads library',
				   metavar='LIB'),
		]
		return options

	def apply_user_defaults(self, tool):
		""" Set default values for user options that haven't been set yet. """
		
		resource_file = os.path.join(self.project.root_dir,'src','ads.qrc')
		print("Adding resource file to qmake project: ", resource_file)
		self.builder_settings.append('RESOURCES += '+resource_file)
		
		if self.ads_lib is not None:
		    self.libraries.append(self.ads_lib)
		    
		if self.ads_incdir is not None:
		    self.include_dirs.append(self.ads_incdir)
		    
		if self.ads_libdir is not None:
		    self.library_dirs.append(self.ads_libdir)
		
		super().apply_user_defaults(tool)
